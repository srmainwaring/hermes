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


const int N = 1000000;

int main() {

  diemer::Timer<double> timer;

  /// Create datatype to be used to write data in the Table and based on the struct MyRecord
  hid_t datatype = H5Tcreate(H5T_COMPOUND, sizeof(MyRecord));

  /// Adding fields to the datatype
  H5Tinsert(datatype, "Field 1", HOFFSET (MyRecord, f1), H5T_NATIVE_INT);
  H5Tinsert(datatype, "Field 2", HOFFSET (MyRecord, f2), H5T_NATIVE_DOUBLE);
  H5Tinsert(datatype, "Field 3", HOFFSET (MyRecord, f3), H5T_NATIVE_INT);


  /// Playing around with memory layout of the datatype
//  std::cout << sizeof(MyRecord) << std::endl;
//  std::cout << H5Tget_size(datatype) << std::endl;
  std::cout << H5Tget_size(H5T_NATIVE_INT) << std::endl;
  std::cout << H5Tget_size(H5T_NATIVE_DOUBLE) << std::endl;
  std::cout << H5Tget_size(H5T_NATIVE_INT) << std::endl;
//  std::cout << HOFFSET (MyRecord, f3) << std::endl;







  exit(EXIT_SUCCESS);


  /// Creating the new hdf5 file
  hid_t file_id = H5Fcreate("essai.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id < 0)
    fprintf(stderr, "Couldn't create file.\n");

  herr_t err;

//  /// Prepare property list to set compression, randomly use deflate with compression level 5
//  hid_t plistID = H5Pcreate(H5P_DATASET_CREATE);
//  herr_t err = H5Pset_deflate(plistID, 5);
//  if (err < 0)
//    fprintf(stderr, "Error setting compression level.");

  /// Create a fixed length packet table
////  FL_PacketTable table(file_id, "My Table", datatype, 100, plistID);
//  FL_PacketTable table(file_id, "My Table", datatype, 100, H5P_DEFAULT);
//  if (!table.IsValid())
//    fprintf(stderr, "Unable to create packet table.");

  hid_t table_id = H5PTcreate_fl(file_id, "My Table", datatype, 100, 5);

  // Close the table
  H5PTclose(table_id);


  /// Closing the file
  err = H5Fclose(file_id);
  if (err < 0)
    fprintf(stderr, "Failed to close file.\n");

  /// Do other stuffs...
  /// ...


  /// Opening the file
  file_id = H5Fopen("essai.h5", H5F_ACC_RDWR, H5P_DEFAULT);
  hid_t table_loc_id = H5PTopen(file_id, "My Table");

  /// Building a new record
  MyRecord record;
  record.f1 = rand() % 100;
  record.f2 = (double)rand() / RAND_MAX;
  record.f3 = rand() % 100;


  timer.start();

  int i = 0;
  while (i < N) {

    /*
     * Remarques de performance
     *
     * Clairement, ouvrir et fermer les fichiers et tables resulte en une tres forte baisse de perf
     * Par contre, si on ne le fait pas, on risque reellement en cas de crash d'obtenir un fichier corrompu
     * Il faut donc proteger contre les crash
     */

    /// Appending the record to the packet table
    err = H5PTappend(table_loc_id, 1, &record);
    if (err < 0)
      fprintf(stderr, "Error adding record.");

//    err = H5Fflush(file_id, H5F_SCOPE_LOCAL); // FIXME: Tres grosse perte de perf...

    i++;
  }

  // Close the table
  H5PTclose(table_loc_id);

  /// Closing the file
  err = H5Fclose(file_id);
  if (err < 0)
    fprintf(stderr, "Failed to close file.\n");



  timer.stop();
  std::cout << "I/O took: " << timer() << " seconds for " << N << " records in the table" << std::endl;

//  /// Closing the file
//  err = H5Fclose(file_id);
//  if (err < 0)
//    fprintf(stderr, "Failed to close file.\n");

  return 0;
};