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
    : m_info {}
    , m_field(0)
    , m_sampleType(SampleType::Unknown)
    , m_endianness(Endian::Invalid)
{
}

VolumeData::~VolumeData() = default;

bool VolumeData::read(std::string const& inf)
{
    using namespace std;
    namespace fs = std::filesystem;

    m_info.read(inf);

    if (!checkInfoIntegrity()) {
        return false;
    }

    auto const resolution = *m_info.resolution;
    auto const voxelSize = *m_info.voxelSize;
    string const& sampleTypeStr = *m_info.sampleType;
    string const& endiannessStr = *m_info.endianness;
    string const& rawFilePath = *m_info.rawFile;

    if (resolution.x <= 0 || resolution.y <= 0 || resolution.z <= 0) {
        Logger::error(R"(%s: "resolution" values should be positive.)", inf.c_str());
        return false;
    }

    if (voxelSize.x <= 0.0f || voxelSize.y <= 0.0f || voxelSize.z <= 0.0f) {
        Logger::error(R"(%s: "voxel.size" values should be positive.)", inf.c_str());
        return false;
    }

    if (sampleTypeStr == "unsigned char") {
        m_sampleType = SampleType::UnsignedChar;
    } else if (sampleTypeStr == "unsigned short") {
        m_sampleType = SampleType::UnsignedShort;
    } else if (sampleTypeStr == "float") {
        m_sampleType = SampleType::Float;
    } else if (sampleTypeStr == "short") {
        m_sampleType = SampleType::Short;
    } else {
        Logger::error("Unknown sample type: \"%s\"", sampleTypeStr.c_str());
        return false;
    }

    if (endiannessStr == "little") {
        m_endianness = Endian::Little;
    } else if (endiannessStr == "big") {
        m_endianness = Endian::Big;
    } else {
        Logger::error("Invalid endianness: \"%s\"", endiannessStr.c_str());
        return false;
    }

    m_field.clear();
    ifstream rawFile(rawFilePath, ios::binary);
    if (rawFile.fail()) {
        Logger::error(R"(Failed to read RAW file: "%s")", rawFilePath.c_str());
        return false;
    }

    auto rawFileSize = fs::file_size(rawFilePath);
    switch (m_sampleType) {
    case SampleType::UnsignedChar: {
        vector<uint8_t> buffer(rawFileSize);
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        m_field.assign(buffer.begin(), buffer.end());
    } break;
    case SampleType::UnsignedShort: {
        vector<uint16_t> buffer(rawFileSize / sizeof(uint16_t));
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        if (m_endianness == Endian::Big) {
            for (auto& n : buffer) {
                n = endianBig2Little<unsigned short>(n);
            }
        }
        m_field.assign(buffer.begin(), buffer.end());
    } break;
    case SampleType::Float: {
        vector<float> buffer(rawFileSize / sizeof(float));
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        if (m_endianness == Endian::Big) {
            for (auto& n : buffer) {
                n = endianBig2Little<float>(n);
            }
        }
        m_field.assign(buffer.begin(), buffer.end());
    } break;
    case SampleType::Short: {
        vector<int16_t> buffer(rawFileSize / sizeof(int16_t));
        rawFile.read(reinterpret_cast<char*>(buffer.data()), rawFileSize);
        if (m_endianness == Endian::Big) {
            for (auto& n : buffer) {
                n = endianBig2Little<short>(n);
            }
        }
        m_field.assign(buffer.begin(), buffer.end());
    } break;
    default:
        break;
    }

    rawFile.close();

    return true;
}

bool VolumeData::checkInfoIntegrity() const
{
    return m_info.resolution && m_info.voxelSize && m_info.endianness && m_info.sampleType && m_info.rawFile;
}

glm::ivec3 VolumeData::resolution() const
{
    assert(m_info.resolution);
    return *m_info.resolution;
}

glm::vec3 VolumeData::voxelSize() const
{
    assert(m_info.voxelSize);
    return *m_info.voxelSize;
}

glm::vec3 VolumeData::volumeSize() const
{
    assert(m_info.resolution && m_info.voxelSize);
    auto const& reso = *m_info.resolution;
    auto const& vs = *m_info.voxelSize;
    return {
        reso.x * vs.x,
        reso.y * vs.y,
        reso.z * vs.z,
    };
}

std::vector<float>& VolumeData::data()
{
    return m_field;
}

std::vector<float> const& VolumeData::data() const
{
    return m_field;
}

float VolumeData::min() const
{
    return *std::min_element(m_field.begin(), m_field.end());
}

float VolumeData::max() const
{
    return *std::max_element(m_field.begin(), m_field.end());
}

std::size_t VolumeData::index(int x, int y, int z) const
{
    assert(m_info.resolution);

    auto const& reso = *m_info.resolution;
    return static_cast<std::size_t>(x + y * reso.x + z * reso.y * reso.x);
}

float VolumeData::value(int x, int y, int z) const
{
    return m_field[index(x, y, z)];
}

float VolumeData::value(std::size_t index) const
{
    return m_field[index];
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
