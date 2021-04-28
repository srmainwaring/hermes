//
// Created by frongere on 28/04/2021.
//

#include "hdf5.h"
#include <filesystem>
#include "hermes/hermes.h"
#include "diemer/Timer.h"


const int N = 1000000;
//const int FLUSH_PERIOD = 50;
const int COMPRESSION_LEVEL = 0; // TODO: injecter dans les messages hermes...


struct HumanReadable {
  // https://en.cppreference.com/w/cpp/filesystem/directory_entry/file_size
  uintmax_t size{};

  template<typename Os>
  friend Os &operator<<(Os &os, HumanReadable hr) {
    int i{};
    double mantissa = hr.size;
    for (; mantissa >= 1024.; ++i) {
      mantissa /= 1024.;
    }
    mantissa = std::ceil(mantissa * 10.) / 10.;
    os << mantissa << "BKMGTPE"[i];
    return i == 0 ? os : os << "B (" << hr.size << ')';
  }
};

struct Data {
  int _i = 0;
  double _d = 0.;
  int _j = 0;

  void renew() {
    _i = rand() % 100;
    _d = (double) rand() / RAND_MAX;
    _j = rand() % 100;
  }
};


int main() {

  // TODO: simplifier dans un header approprie. On ne veut pas d'appel direct hdf5 ici !
  const std::string h5file("toto.h5");

  // HDF5 file creation
  hid_t h5file_id = H5Fcreate(h5file.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (h5file_id < 0)
    fprintf(stderr, "Couldn't create file.\n");


  // Declaring the data struct to be used during the hdf5 filling
  Data data;

  // hermes message definition
  hermes::Message msg("essai", "");

  msg.AddField("_i", "_", "first integer", &data._i);
  msg.AddField("_d", "_", "a double", &data._d);
  msg.AddField("_j", "_", "second integer", &data._j);

  msg.AddHDF5Serializer(h5file_id, "/");

  msg.Initialize();


  diemer::Timer<double> timer;
  timer.start();

  int i = 0;
  int iflush = 1;
  while (i < N) {

    // New set of data
    data.renew();

    // Serialization
    msg.Serialize();

    msg.Send();

    iflush++;
    i++;
  }

  timer.stop();
  std::cout << "HDF5 I/O took: "
            << timer() << " seconds for "
            << N << " records composed of 3 fields of data "
            << ", file size is "
            << HumanReadable{std::filesystem::file_size("essai.h5")}
            << "\t[COMPRESSION LEVEL: " << COMPRESSION_LEVEL << "]"
            << std::endl;

  msg.Finalize();

  /// Closing the file
  herr_t err = H5Fclose(h5file_id);
  if (err < 0)
    fprintf(stderr, "Failed to close file.\n");

  return 0;
}