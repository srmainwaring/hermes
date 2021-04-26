//
// Created by frongere on 21/04/2021.
//

/*
 * Sources:
 *
 * http://santoshdarekar-hdf5.blogspot.com/2011/02/compound-data-in-packet-table.html
 * https://forum.hdfgroup.org/t/avoiding-a-corrupted-hdf5-file-or-be-able-to-recover-it/5441/2
 *
 */


#include <cstdlib>
#include <iostream>

#include "H5PacketTable.h"

#include "hermes/hermes.h"

#include "diemer/Timer.h"

const int N = 1300000;
const int FLUSH_PERIOD = 50;
const int COMPRESSION_FACTOR = 1;



/*
 * On fait le choix de ne pas reposer (le moins possible) sur des interfaces high-level
 * Ceci a but de performances
 * On utilise donc uniquement l'API C pure de HDF5, pas ses wrappers C++
 *
 */



/*
 * TODO: on voudrait s'affanchir de la definition de structures pour declarer des records des tables
 * En effet, on aimerait pouvoir juste declarer des arrays de bytes comme record, ie, une suite dans padding de
 * fields. C'est plus raccord avec la maniere de fonctionner d'hermes.
 *
 * Dans les exemples fournis par HDF5, il est toujours utilise une structure pour specifier en memoire un type
 * COMPOUND. Pour creer le type HDF5_COMPOUND et specifier sa taille, il est recommande d'utiliser sizeof sur la
 * structure afin d'avoir une determination de la taille de la struct portable. En effet, suivant l'architecture,
 * le compilateur etc... on peut avoir des tailles en memoire de structures differentes (padding). Ensuite, pour
 * specifier les champs du COMPOUND, on utilise la macro HOFFSET(s, f) (qui est en fait offsetof(s, f)) qui permet
 * d'obtenir l'offset de chaque champ de la structure de maniere exacte de maniere portable car cela prnd en compte
 * le padding utilise par l'architecture et le compilateur.
 *
 * Le probleme ici avec hermes est qu'on voudrait eviter de specifier une structure pour chaque message car ca revient
 * a changer substantiellement le fonctionnement d'hermes et aurait une emprunte non nulle a un datalog en HDF5.
 * L'objectif est donc d'etre capables de construire des bytes arrays, sans padding qui soient compris par le COMPOUND.
 * On veut reposer sur les tailles des types NATIVE de hdf5 qui assurent la portabilite et le abouter dans un tableau
 * de bytes a utiliser en lieu et place d'une structure C. Les verrous pour y arriver sont:
 *
 * 1 - on doit etre capables d'abouter des champs de message hermes sous forme de tableaux de bytes
 * 2 - le COMPOUND datatype HDF5 est defini a l'aide d'offsets qui correspondent et bases sur les types natifs portables
 * 3 - on a suffisamment d'introspection dans les fichiers HDF5 pour etre capables de lire les records sans connaitres
 * leur layout memoire sur le disque ---> HDFview sait le faire !!! On doit savoir le faire !
 * 4 - il faut que les IO soient rapides
 *
 * Autre aspect crucial est d'etre tolerant quant aux arrets brutaux de programmes (crash ou arret de type CTRL-C)
 * Dans ce type de cas, si on a un arret au moment d'un write HDF5, il est possible de se retrouver avec un fichier
 * HDF5 corrompu qu'il ne sera pas possible de lire. C'est impenssable !! En situation "labo" on peut s'en accomoder
 * parfois (relancer...) mais dans la vrai vie, ce n'est pas possible. Donc soit on a une solution robuste qui permet
 * d'eviter/limiter la corruption de fichier, soit la solution HDF5 n'est pas viable.
 * Dans les pistes de solutions trouvees on a:
 * - du flush regulier --> systematique c'est extremement couteux et met en peril l'interet de H5PT...
 * - du flush controle (on accepte de pouvoir perdre quelques pas de temps) -> voir un optimum...
 * - de l'utilsation de fichiers hdf5 en mode SWMR et de la correction de fichier via l'outil h5clean -> a tester !
 *
 *
 */
struct MyRecord {
  int f0;
  double f1;
  int f2;
  double f3;
  int f4;
  double f5;
  int f6;
  double f7;
  int f8;
  double f9;
};

MyRecord get_random_record() {
  MyRecord record;
  record.f0 = rand() % 100;
  record.f1 = (double) rand() / RAND_MAX;
  record.f2 = rand() % 100;
  record.f3 = (double) rand() / RAND_MAX;
  record.f4 = rand() % 100;
  record.f5 = (double) rand() / RAND_MAX;
  record.f6 = rand() % 100;
  record.f7 = (double) rand() / RAND_MAX;
  record.f8 = rand() % 100;
  record.f9 = (double) rand() / RAND_MAX;
  return record;
}


void test_hdf5() {
  /// Create datatype to be used to write data in the Table and based on the struct MyRecord
  hid_t datatype = H5Tcreate(H5T_COMPOUND, sizeof(MyRecord));

  /// Adding fields to the datatype
  H5Tinsert(datatype, "Field 0", HOFFSET(MyRecord, f0), H5T_NATIVE_INT);
  H5Tinsert(datatype, "Field 1", HOFFSET(MyRecord, f1), H5T_NATIVE_DOUBLE);
  H5Tinsert(datatype, "Field 2", HOFFSET(MyRecord, f2), H5T_NATIVE_INT);
  H5Tinsert(datatype, "Field 3", HOFFSET(MyRecord, f3), H5T_NATIVE_DOUBLE);
  H5Tinsert(datatype, "Field 4", HOFFSET(MyRecord, f4), H5T_NATIVE_INT);
  H5Tinsert(datatype, "Field 5", HOFFSET(MyRecord, f5), H5T_NATIVE_DOUBLE);
  H5Tinsert(datatype, "Field 6", HOFFSET(MyRecord, f6), H5T_NATIVE_INT);
  H5Tinsert(datatype, "Field 7", HOFFSET(MyRecord, f7), H5T_NATIVE_DOUBLE);
  H5Tinsert(datatype, "Field 8", HOFFSET(MyRecord, f8), H5T_NATIVE_INT);
  H5Tinsert(datatype, "Field 9", HOFFSET(MyRecord, f9), H5T_NATIVE_DOUBLE);


  /// Playing around with memory layout of the datatype
//  std::cout << sizeof(MyRecord) << std::endl;
//  std::cout << H5Tget_size(datatype) << std::endl;
  std::cout << H5Tget_size(H5T_NATIVE_INT) << std::endl;
  std::cout << H5Tget_size(H5T_NATIVE_DOUBLE) << std::endl;
  std::cout << H5Tget_size(H5T_NATIVE_INT) << std::endl;
//  std::cout << HOFFSET (MyRecord, f3) << std::endl;


  // TODO: gerer le fichier en SWMR de maniere a pouvoir reparer un fichier corrompu (crash programme sans H5Fclose)
  // Reparation avec l'outil h5clean
  // voir: https://forum.hdfgroup.org/t/avoiding-a-corrupted-hdf5-file-or-be-able-to-recover-it/5441/2

  /// Creating the new hdf5 file
  hid_t file_id = H5Fcreate("essai.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id < 0)
    fprintf(stderr, "Couldn't create file.\n");

  herr_t err;

  /// Create a fixed length packet table
  hid_t table_id = H5PTcreate_fl(file_id, "My Table", datatype, 100, COMPRESSION_FACTOR);

  /// Close the table
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

  diemer::Timer<double> timer;
  timer.start();

  int i = 0;
  int iflush = 1;
  while (i < N) {

    /*
     * Remarques de performance
     *
     * Clairement, ouvrir et fermer les fichiers et tables resulte en une tres forte baisse de perf
     * Par contre, si on ne le fait pas, on risque reellement en cas de crash d'obtenir un fichier corrompu
     * Il faut donc proteger contre les crash
     * Un flush a une certaine frequence, permet dans les tests effectues de pouvoir recuperer le fichier dans l'etat
     * du dernier flush
     */

    /// Building a new record
    auto record = get_random_record();

    /// Appending the record to the packet table
    err = H5PTappend(table_loc_id, 1, &record);
    if (err < 0)
      fprintf(stderr, "Error adding record.");

    /// Flushing buffer at FLUSH_PERIOD rate
    if (iflush == FLUSH_PERIOD) {
      /*
       * FIXME: Grosse perte de perf si fait trop souvent
       * Il y a certainement un optimum suivant la quantite de donnees a enregistrer
       * Peut-on avoir une flush period adaptative travaillant avec les timings ?
       * On pourrait placer la flush period dans le fichier de config frydom
       *
       */
      err = H5Fflush(file_id, H5F_SCOPE_LOCAL);

      iflush = 1;
    }

    iflush++;
    i++;
  }
  err = H5Fflush(file_id, H5F_SCOPE_LOCAL);

  // Close the table
  H5PTclose(table_loc_id);

  /// Closing the file
  err = H5Fclose(file_id);
  if (err < 0)
    fprintf(stderr, "Failed to close file.\n");

  timer.stop();
  std::cout << "HDF5 I/O took: "
            << timer() << " seconds for "
            << N << " records composed of 10 fields of data"
            << std::endl;


  //  /// Closing the file
//  err = H5Fclose(file_id);
//  if (err < 0)
//    fprintf(stderr, "Failed to close file.\n");

}

void test_csv() {
  hermes::Message msg("essai", "");

  auto record = get_random_record();

  msg.AddField("Field 0", "_", "", &record.f0);
  msg.AddField("Field 1", "_", "", &record.f1);
  msg.AddField("Field 2", "_", "", &record.f2);
  msg.AddField("Field 3", "_", "", &record.f3);
  msg.AddField("Field 4", "_", "", &record.f4);
  msg.AddField("Field 5", "_", "", &record.f5);
  msg.AddField("Field 6", "_", "", &record.f6);
  msg.AddField("Field 7", "_", "", &record.f7);
  msg.AddField("Field 8", "_", "", &record.f8);
  msg.AddField("Field 9", "_", "", &record.f9);

  msg.AddCSVSerializer("essai.csv");

  msg.Initialize();

  diemer::Timer<double> timer;
  timer.start();


  int i = 0;
  int iflush = 1;
  while (i < N) {

    /// Building a new record
    record = get_random_record();

    /// Appending the record to the CSV file
    msg.Serialize();

    /// Flushing buffer at FLUSH_PERIOD rate
    if (iflush == FLUSH_PERIOD) {
      msg.Send();

      iflush = 1;
    }

    iflush++;
    i++;
  }



  timer.stop();
  std::cout << "CSV I/O took: "
            << timer() << " seconds for "
            << N << " records composed of 10 fields of data"
            << std::endl;
}



int main() {

  test_hdf5();

  test_csv();




  return 0;
};
