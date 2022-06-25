#ifndef VOLUME_DATA_H
#define VOLUME_DATA_H

#include <optional>
#include <string>
#include <vector>

#include <glm/glm.hpp>

class VolumeData
{
    struct Info {
        std::optional<std::string> rawFile;
        std::optional<std::string> sampleType;
        std::optional<std::string> endianness;
        std::optional<glm::ivec3> resolution;
        std::optional<glm::vec3> voxelSize;
        void read(std::string const& file);
    };

    enum class SampleType { Unknown, UnsignedChar, UnsignedShort, Float, Short };
    enum class Endian { Invalid, Little, Big };

    VolumeData::Info m_info;
    std::vector<float> m_field;
    SampleType m_sampleType;
    Endian m_endianness;

public:
    VolumeData();
    ~VolumeData();
    VolumeData(VolumeData const&) = delete;
    VolumeData& operator=(VolumeData const&) = delete;

    [[nodiscard]] bool read(std::string const& inf);
    glm::ivec3 resolution() const;
    glm::vec3 voxelSize() const;
    glm::vec3 volumeSize() const;
    std::vector<float>& data();
    std::vector<float> const& data() const;
    float min() const;
    float max() const;
    std::size_t index(int x, int y, int z) const;
    float value(int x, int y, int z) const;
    float value(std::size_t index) const;

private:
    bool checkInfoIntegrity() const;
    template <typename T>
    inline T endianBig2Little(T const& value);
};

template <typename T>
T VolumeData::endianBig2Little(T const& value)
{
    constexpr std::size_t count = sizeof(T);

    T result;

    char const* const big = reinterpret_cast<char const*>(&value);
    char* const little = reinterpret_cast<char*>(&result);
    /**
     * Example: sizeof(float) is 4
     *
     * little[0] = big[3]
     * little[1] = big[2]
     * little[2] = big[1]
     * little[3] = big[0]
     */
    for (std::size_t i = 0; i < count; ++i) {
        little[i] = big[count - (i + 1)];
    }

    return result;
}

#endif
