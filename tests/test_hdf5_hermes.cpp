//
// Created by frongere on 28/04/2021.
//

#include "hdf5.h"

#include "hermes/hermes.h"


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
  msg.AddField("_i", "_", "second integer", &data._j);

  msg.AddHDF5Serializer(h5file_id, "/");

  msg.Initialize();







  /// Closing the file
  herr_t err = H5Fclose(h5file_id);
  if (err < 0)
    fprintf(stderr, "Failed to close file.\n");

  return 0;
}