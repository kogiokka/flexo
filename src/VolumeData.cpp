#include "VolumeData.hpp"
#include "Util.hpp"
#include "common/Logger.hpp"

#include <toml.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>

VolumeData::VolumeData()
    : info_ {}
    , field_(0)
    , sampleType_(SampleType::Unknown)
    , endianness_(Endian::Invalid)
{
}

VolumeData::~VolumeData() = default;

bool VolumeData::read(std::string const& inf)
{
    using namespace std;
    namespace fs = std::filesystem;

    info_.read(inf);

    if (!checkInfoIntegrity()) {
        return false;
    }

    auto const resolution = *info_.resolution;
    auto const voxelSize = *info_.voxelSize;
    string const& sampleTypeStr = *info_.sampleType;
    string const& endiannessStr = *info_.endianness;
    string const& rawFilePath = *info_.rawFile;

    if (resolution.x <= 0 || resolution.y <= 0 || resolution.z <= 0) {
        Logger::error(R"(%s: "resolution" values should be positive.)", inf.c_str());
        return false;
    }

    if (voxelSize.x <= 0.0f || voxelSize.y <= 0.0f || voxelSize.z <= 0.0f) {
        Logger::error(R"(%s: "voxel.size" values should be positive.)", inf.c_str());
        return false;
    }

    if (sampleTypeStr == "unsigned char") {
        sampleType_ = SampleType::UnsignedChar;
    } else if (sampleTypeStr == "unsigned short") {
        sampleType_ = SampleType::UnsignedShort;
    } else if (sampleTypeStr == "float") {
        sampleType_ = SampleType::Float;
    } else if (sampleTypeStr == "short") {
        sampleType_ = SampleType::Short;
    } else {
        Logger::error("Unknown sample type: \"%s\"", sampleTypeStr.c_str());
        return false;
    }

    if (endiannessStr == "little") {
        endianness_ = Endian::Little;
    } else if (endiannessStr == "big") {
        endianness_ = Endian::Big;
    } else {
        Logger::error("Invalid endianness: \"%s\"", endiannessStr.c_str());
        return false;
    }

    field_.clear();
    ifstream rawFile(rawFilePath, ios::binary);
    if (rawFile.fail()) {
        Logger::error(R"(Failed to read RAW file: "%s")", rawFilePath.c_str());
        return false;
    }

    auto rawFileSize = fs::file_size(rawFilePath);
    switch (sampleType_) {
    case SampleType::UnsignedChar: {
        vector<uint8_t> buffer(rawFileSize);
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        field_.assign(buffer.begin(), buffer.end());
    } break;
    case SampleType::UnsignedShort: {
        vector<uint16_t> buffer(rawFileSize / sizeof(uint16_t));
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        if (endianness_ == Endian::Big) {
            for (auto& n : buffer) {
                n = endianBig2Little<unsigned short>(n);
            }
        }
        field_.assign(buffer.begin(), buffer.end());
    } break;
    case SampleType::Float: {
        vector<float> buffer(rawFileSize / sizeof(float));
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        if (endianness_ == Endian::Big) {
            for (auto& n : buffer) {
                n = endianBig2Little<float>(n);
            }
        }
        field_.assign(buffer.begin(), buffer.end());
    } break;
    case SampleType::Short: {
        vector<int16_t> buffer(rawFileSize / sizeof(int16_t));
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        if (endianness_ == Endian::Big) {
            for (auto& n : buffer) {
                n = endianBig2Little<short>(n);
            }
        }
        field_.assign(buffer.begin(), buffer.end());
    } break;
    default:
        break;
    }

    rawFile.close();

    return true;
}

bool VolumeData::checkInfoIntegrity() const
{
    return info_.resolution && info_.voxelSize && info_.endianness && info_.sampleType && info_.rawFile;
}

glm::ivec3 VolumeData::resolution() const
{
    assert(info_.resolution);
    return *info_.resolution;
}

glm::vec3 VolumeData::voxelSize() const
{
    assert(info_.voxelSize);
    return *info_.voxelSize;
}

glm::vec3 VolumeData::volumeSize() const
{
    assert(info_.resolution && info_.voxelSize);
    auto const& reso = *info_.resolution;
    auto const& vs = *info_.voxelSize;
    return {
        reso.x * vs.x,
        reso.y * vs.y,
        reso.z * vs.z,
    };
}

std::vector<float>& VolumeData::data()
{
    return field_;
}

std::vector<float> const& VolumeData::data() const
{
    return field_;
}

float VolumeData::min() const
{
    return *std::min_element(field_.begin(), field_.end());
}

float VolumeData::max() const
{
    return *std::max_element(field_.begin(), field_.end());
}

std::size_t VolumeData::index(int x, int y, int z) const
{
    assert(info_.resolution);

    auto const& reso = *info_.resolution;
    return static_cast<std::size_t>(x + y * reso.x + z * reso.y * reso.x);
}

float VolumeData::value(int x, int y, int z) const
{
    return field_[index(x, y, z)];
}

float VolumeData::value(std::size_t index) const
{
    return field_[index];
}

void VolumeData::Info::read(std::string const& file)
{
    namespace fs = std::filesystem;

    toml::table tbl = toml::parse_file(file);

    auto const& resoNode = tbl["resolution"];
    auto const& voxelNode = tbl["voxel"];
    {
        auto const x = resoNode["x"].value<int>();
        auto const y = resoNode["y"].value<int>();
        auto const z = resoNode["z"].value<int>();
        try {
            resolution = glm::ivec3(x.value(), y.value(), z.value());
        } catch (std::bad_optional_access const& e) {
            resolution = std::nullopt;
            Logger::error("Missing resolution value.");
        }
    }
    {
        auto const x = voxelNode["size"]["x"].value<float>();
        auto const y = voxelNode["size"]["y"].value<float>();
        auto const z = voxelNode["size"]["z"].value<float>();
        try {
            voxelSize = glm::vec3(x.value(), y.value(), z.value());
        } catch (std::bad_optional_access const& e) {
            voxelSize = std::nullopt;
            Logger::error("Missing voxel size value.");
        }
    }

    endianness = voxelNode["endianness"].value_or("little");
    sampleType = voxelNode["sample_type"].value<std::string>();

    fs::path const tomlpath = fs::path(file);
    fs::path const rawpath = tbl["raw_file"].value_or(tomlpath.filename().replace_extension(".raw").string());
    rawFile = (tomlpath.parent_path() / rawpath).string();
}
