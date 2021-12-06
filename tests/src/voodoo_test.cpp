#include <fstream>

#include <gtest/gtest.h>

#include <spitfire/storage/filesystem.h>

#include <libvoodoomm/cImage.h>
#include <libvoodoomm/dds_reader.h>

namespace {

bool IsApproximatelyEqual(uint8_t a, uint8_t b)
{
  // Return true if value a is within the range [b - 1 .. b + 1]
  return (int16_t(a) >= (int16_t(b) - 1)) && (int16_t(a) <= (int16_t(b) + 1));
}

/*bool write_ppm(const std::string& file_path, const voodoo::Image& image)
{
  std::ofstream ofs(file_path);
  if(!ofs.good()) {
    std::cerr<<"Error opening file \""<<file_path<<"\""<<std::endl;
    return false;
  }

  // Write the header
  ofs<<
    "P3\n"<<
    image.width<<' '<<image.height<<"\n"
    "255\n"
  ;

  size_t index = 0;

  if (image.bpp == 24) {
    for (size_t y = 0; y < image.height; y++) {
      for (size_t x = 0; x < image.width; x++) {
        // Output the RGB value for this pixel
        ofs<<std::setw(3)<<static_cast<int>(image.data[index])<<' '
          <<std::setw(3)<<static_cast<int>(image.data[index + 1])<<' '
          <<std::setw(3)<<static_cast<int>(image.data[index + 2]);

        if (x + 1 != image.width) ofs<<' ';

        index += 3;
      }

      ofs<<'\n';
    }
  } else {
    // RGBA
    for (size_t y = 0; y < image.height; y++) {
      for (size_t x = 0; x < image.width; x++) {
        // Output the RGB value for this pixel
        ofs<<std::setw(3)<<static_cast<int>(image.data[index])<<' '
          <<std::setw(3)<<static_cast<int>(image.data[index + 1])<<' '
          <<std::setw(3)<<static_cast<int>(image.data[index + 2]);

        if (x + 1 != image.width) ofs<<' ';

        index += 4;
      }

      ofs<<'\n';
    }
  }

  return true;
}

bool write_png(const std::string& file_path, const voodoo::Image& image, voodoo::PIXELFORMAT format)
{
  voodoo::cImage i;
  i.CreateFromBuffer(image.data.data(), image.width, image.height, format);
  return i.SaveToPNG(file_path);
}*/

}

TEST(Voodoo, TestDDSReader)
{
  voodoo::cImage image;

  struct test_item {
    std::string dds_file;
    voodoo::DDS_IMAGE_TYPE expected_dds_type;
    size_t expected_width;
    size_t expected_height;
    std::string expected_png_file;
  };

  const test_item items[] = {
    { "data/images/dds/italian_curb.dds", voodoo::DDS_IMAGE_TYPE::DXT1, 256, 256, "data/images/dds/italian_curb.expected.png" },
    { "data/images/dds/cobbles.dds", voodoo::DDS_IMAGE_TYPE::DXT1, 1024, 1024, "data/images/dds/cobbles.expected.png" },
    { "data/images/dds/palm_tree.dds", voodoo::DDS_IMAGE_TYPE::DXT3, 1536, 3072, "data/images/dds/palm_tree.expected.png" },
    { "data/images/dds/trees.dds", voodoo::DDS_IMAGE_TYPE::DXT5, 2048, 2048, "data/images/dds/trees.expected.png" },
  };

  for (auto& item : items) {
    const size_t nFileSizeBytes = spitfire::filesystem::GetFileSizeBytes(item.dds_file);

    std::ifstream file(item.dds_file, std::ios::binary);

    std::vector<uint8_t> buffer(nFileSizeBytes);
    ASSERT_TRUE(file.read((char*)buffer.data(), nFileSizeBytes));

    voodoo::DDS_IMAGE_TYPE dds_image_type = voodoo::DDS_IMAGE_TYPE::UNKNOWN;

    EXPECT_TRUE(voodoo::read_dds(buffer, dds_image_type, image));

    std::string type = "Unkown";
    if (dds_image_type == voodoo::DDS_IMAGE_TYPE::RGB) {
      type = "RGB";
      //write_png(item.dds_file + ".png", image, voodoo::PIXELFORMAT::R8G8B8);
      //write_ppm(item.dds_file + ".ppm", image);
    } else if (dds_image_type == voodoo::DDS_IMAGE_TYPE::DXT1) {
      type = "DXT1";
      //write_png(item.dds_file + ".png", image, voodoo::PIXELFORMAT::R8G8B8);
      //write_ppm(item.dds_file + ".ppm", image);
    } else if (dds_image_type == voodoo::DDS_IMAGE_TYPE::DXT3) {
      type = "DXT3";
      //write_png(item.dds_file + ".png", image, voodoo::PIXELFORMAT::R8G8B8A8);
      //write_ppm(item.dds_file + ".ppm", image);
    } else if (dds_image_type == voodoo::DDS_IMAGE_TYPE::DXT5) {
      type = "DXT5";
      //write_png(item.dds_file + ".png", image, voodoo::PIXELFORMAT::R8G8B8A8);
      //write_ppm(item.dds_file + ".ppm", image);
    }
  
    std::cout<<item.dds_file<<" "<<type<<" "<<image.GetWidth()<<"x"<<image.GetHeight()<<" "<<image.GetBytesPerPixel()<<" bpp"<<std::endl;

    EXPECT_EQ(item.expected_dds_type, dds_image_type);
    EXPECT_EQ(item.expected_width, image.GetWidth());
    EXPECT_EQ(item.expected_height, image.GetHeight());


    // Compare the loaded image data to the image data from the expected png
    voodoo::cImage image_expected;
    ASSERT_TRUE(image_expected.LoadFromFile(item.expected_png_file));

    const size_t actual_bytes = image.GetWidth() * image.GetHeight() * image.GetBytesPerPixel();
    const size_t expected_bytes = image_expected.GetWidth() * image_expected.GetHeight() * image_expected.GetBytesPerPixel();
  
    ASSERT_EQ(actual_bytes, expected_bytes);

    const uint8_t* image_data = image.GetPointerToBuffer();
    const uint8_t* expected_image_data = image_expected.GetPointerToBuffer();

    if (
      (dds_image_type == voodoo::DDS_IMAGE_TYPE::RGB) || 
      (dds_image_type == voodoo::DDS_IMAGE_TYPE::DXT1)
    ) {
      // RGB
      for (size_t i = 0; i < actual_bytes; i += 3) {
        ASSERT_EQ(image_data[i], expected_image_data[i]) << "Byte is incorrect at " << i;
        ASSERT_EQ(image_data[i + 1], expected_image_data[i + 1]) << "Byte is incorrect at " << i + 1;
        ASSERT_EQ(image_data[i + 2], expected_image_data[i + 2]) << "Byte is incorrect at " << i + 2;
      }
    } else {
      // RGBA
      for (size_t i = 0; i < actual_bytes; i += 4) {
        ASSERT_EQ(image_data[i], expected_image_data[i]) << "Byte is incorrect at " << i;
        ASSERT_EQ(image_data[i + 1], expected_image_data[i + 1]) << "Byte is incorrect at " << i + 1;
        ASSERT_EQ(image_data[i + 2], expected_image_data[i + 2]) << "Byte is incorrect at " << i + 2;

        // The alpha channel conversion is not quite exact
        ASSERT_TRUE(IsApproximatelyEqual(image_data[i + 3], expected_image_data[i + 3])) << "Byte is incorrect at " << i + 3;
      }
    }
  }
}
