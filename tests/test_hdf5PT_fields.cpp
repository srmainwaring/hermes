//
// Created by frongere on 21/04/2021.
//

/*
 * Sources:
 *
 * http://santoshdarekar-hdf5.blogspot.com/2011/02/compound-data-in-packet-table.html
 *
 */

#include <cstdlib>
#include <iostream>
#include "H5PacketTable.h"
#include "diemer/Timer.h"

struct MyRecord {
  int f1;
  double f2;
  int f3;
};


int main() {

  diemer::Timer<double> timer;

  /// Create data type to be used to write data in the Table and based on the struct MyRecord
  hid_t datatype = H5Tcreate(H5T_COMPOUND, sizeof(MyRecord));

  herr_t status;
  status = H5Tinsert(datatype, "Field 1",
                     HOFFSET (MyRecord, f1), H5T_NATIVE_INT);

  status = H5Tinsert(datatype, "Field 2",
                     HOFFSET (MyRecord, f2), H5T_NATIVE_DOUBLE);

  status = H5Tinsert(datatype, "Field 3",
                     HOFFSET (MyRecord, f3), H5T_NATIVE_INT);

  /// Creating the new hdf5 file
  hid_t file_id = H5Fcreate("essai.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id < 0)
    fprintf(stderr, "Couldn't create file.\n");

  /// Preapare property list to set compression, randomly use deflate with compression level 5
  hid_t plistID = H5Pcreate(H5P_DATASET_CREATE);
  herr_t err = H5Pset_deflate(plistID, 5);
  if (err < 0)
    fprintf(stderr, "Error setting compression level.");

  /// Create a fixed length packet table
  FL_PacketTable ptable(file_id, plistID, "My Table", datatype, 100);
  if (!ptable.IsValid())
    fprintf(stderr, "Unable to create packet table.");


  const int n = 1000000;
  int i = 0;
  timer.start();
  while (i < n) {
    /// Opening the file
    // TODO

    /// Building a new record
    MyRecord record;
    record.f1 = rand() % 100;
    record.f2 = (double)rand() / RAND_MAX;
    record.f3 = rand() % 100;

    /// Appending the record to the packet table
    err = ptable.AppendPacket(&record);
    if (err < 0)
      fprintf(stderr, "Error adding record.");

    /// Closing the file
//    err = H5Fclose(file_id);
//    if (err < 0)
//      fprintf(stderr, "Failed to close file.\n");

    i++;
  }
  timer.stop();
  std::cout << "I/O took: " << timer() << " seconds for " << 3*n << " records in the table" << std::endl;

  /// Closing the file
  err = H5Fclose(file_id);
  if (err < 0)
    fprintf(stderr, "Failed to close file.\n");

  return 0;
};