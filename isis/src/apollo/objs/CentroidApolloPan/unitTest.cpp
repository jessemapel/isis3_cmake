#include <iostream>

#include "CentroidApolloPan.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"

#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  try {
    cout << "Unit test for Isis::CentroidApolloPan" << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }
}