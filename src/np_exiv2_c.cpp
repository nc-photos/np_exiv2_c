#include "../include/np_exiv2_c.h"
#include "./log.h"
#include "./reader.h"
#include <cstdio>
#include <cstring>
#include <exception>

using namespace std;

namespace {

void convertCppType(Exiv2Metadatum *that,
                    const np_exiv2::reader::Metadatum &obj) {
  auto tag_key = (char *)malloc(obj.tag_key.length() + 1);
  strcpy(tag_key, obj.tag_key.c_str());
  that->tag_key = tag_key;
  that->type_id = (Exiv2TypeId)obj.type_id;
  auto data = (uint8_t *)malloc(obj.data.size());
  memcpy(data, obj.data.data(), obj.data.size());
  that->data = data;
  that->size = obj.data.size();
  that->count = obj.count;
}

void convertCppType(Exiv2ReadResult *that,
                    const np_exiv2::reader::Result &obj) {
  that->width = obj.width;
  that->height = obj.height;

  LOG("Converting IPTC data\n");
  auto iptc_data =
      (Exiv2Metadatum *)malloc(obj.iptc_data.size() * sizeof(Exiv2Metadatum));
  auto dst_it = iptc_data;
  for (auto it = obj.iptc_data.begin(); it != obj.iptc_data.end(); ++it) {
    LOG("- Convert %s\n", it->tag_key.c_str());
    convertCppType(dst_it++, *it);
  }
  that->iptc_data = iptc_data;
  that->iptc_count = obj.iptc_data.size();

  LOG("Converting EXIF data\n");
  auto exif_data =
      (Exiv2Metadatum *)malloc(obj.exif_data.size() * sizeof(Exiv2Metadatum));
  dst_it = exif_data;
  for (auto it = obj.exif_data.begin(); it != obj.exif_data.end(); ++it) {
    LOG("- Convert %s\n", it->tag_key.c_str());
    convertCppType(dst_it++, *it);
  }
  that->exif_data = exif_data;
  that->exif_count = obj.exif_data.size();

  LOG("Converting XMP data\n");
  auto xmp_data =
      (Exiv2Metadatum *)malloc(obj.xmp_data.size() * sizeof(Exiv2Metadatum));
  dst_it = xmp_data;
  for (auto it = obj.xmp_data.begin(); it != obj.xmp_data.end(); ++it) {
    LOG("- Convert %s\n", it->tag_key.c_str());
    convertCppType(dst_it++, *it);
  }
  that->xmp_data = xmp_data;
  that->xmp_count = obj.xmp_data.size();
}

void exiv2_metadatum_free(const Exiv2Metadatum *that) {
  free((void *)that->tag_key);
  free((void *)that->data);
}

} // namespace

const Exiv2ReadResult *exiv2_read_file(const char *path) {
  np_exiv2::reader::Reader reader;
  try {
    auto result = reader.read_file(path);
    if (result) {
      LOG("Converting result\n");
      auto cresult = (Exiv2ReadResult *)malloc(sizeof(Exiv2ReadResult));
      convertCppType(cresult, *result);
      LOG("Done\n");
      return cresult;
    }
  } catch (const exception &e) {
    LOG("Exception reading file: %s\n", e.what());
  } catch (...) {
    LOG("Exception reading file\n");
  }
  return nullptr;
}

const Exiv2ReadResult *exiv2_read_buffer(const uint8_t *buffer,
                                         const size_t size) {
  np_exiv2::reader::Reader reader;
  try {
    auto result = reader.read_buffer(buffer, size);
    if (result) {
      LOG("Converting result\n");
      auto cresult = (Exiv2ReadResult *)malloc(sizeof(Exiv2ReadResult));
      convertCppType(cresult, *result);
      LOG("Done\n");
      return cresult;
    }
  } catch (const exception &e) {
    LOG("Exception reading file: %s\n", e.what());
  } catch (...) {
    LOG("Exception reading file\n");
  }
  return nullptr;
}

void exiv2_result_free(const Exiv2ReadResult *that) {
  for (auto it = that->iptc_data; it != that->iptc_data + that->iptc_count;
       ++it) {
    exiv2_metadatum_free(it);
  }
  free((void *)that->iptc_data);
  for (auto it = that->exif_data; it != that->exif_data + that->exif_count;
       ++it) {
    exiv2_metadatum_free(it);
  }
  free((void *)that->exif_data);
  for (auto it = that->xmp_data; it != that->xmp_data + that->xmp_count; ++it) {
    exiv2_metadatum_free(it);
  }
  free((void *)that->xmp_data);
}
