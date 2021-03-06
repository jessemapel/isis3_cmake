/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "LoMediumCamera.h"
#include "LoMediumDistortionMap.h"
#include "LoCameraFiducialMap.h"

#include <QString>

#include "Affine.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * @brief Initialize the LoMedium camera model
   *
   * This constructor uses the Pvl labels for Lunar Orbiter Medium (20 m)
   * resolution images.
   *
   * @param lab Pvl label from a Lunar Orbiter Mecdium image.
   *
   * @throws IException::User - "Unknown focal plane map type:  Labels must
   *                   include fiducials or boresight"
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  LoMediumCamera::LoMediumCamera(Cube &cube) : FramingCamera(cube) {
    NaifStatus::CheckErrors();
    
    m_instrumentNameLong = "Medium Resolution Camera";
    m_instrumentNameShort = "Medium";
    
    // L03 Medium instrument kernel code = -533002
    if (naifIkCode() == -533002) {
      m_spacecraftNameLong = "Lunar Orbiter 3";
      m_spacecraftNameShort = "LO3";
    }
    // LO4 Medium instrument kernel code = -534002
    else if (naifIkCode() == -534002) {
      m_spacecraftNameLong = "Lunar Orbiter 4";
      m_spacecraftNameShort = "LO4";
    }
    // LO5 Medium instrument kernel code = -535002
    else if (naifIkCode() == -535002) {
      m_spacecraftNameLong = "Lunar Orbiter 5";
      m_spacecraftNameShort = "LO5";
    }
    else {
      QString msg = "File does not appear to be a Lunar Orbiter image: ";
      msg += QString::number(naifIkCode());
      msg += " is not a supported instrument kernel code for Lunar Orbiter.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    
    // Get the Instrument label information needed to define the camera for this frame
    Pvl &lab = *cube.label();
    PvlGroup inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString spacecraft = inst["SpacecraftName"];
    QString instId = inst["InstrumentId"];

    LoMediumCamera::FocalPlaneMapType type;
    if(inst.hasKeyword("FiducialSamples")) {
      type = Fiducial;
    }
    else if(inst.hasKeyword("BoresightSample")) {
      type = Boresight;
    }
    else {
      string msg = "Unknown focal plane map type:  ";
      msg += "Labels must include fiducials or boresight";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Turn off the aberration corrections for the instrument position object
    instrumentPosition()->SetAberrationCorrection("NONE");

    // Get the camera characteristics
    SetFocalLength();
    SetPixelPitch();

    // Get the start time in et
    double time = iTime((QString)inst["StartTime"]).Et();

    // Setup focal plane map
    if(type == Fiducial) {
      LoCameraFiducialMap fid(inst, naifIkCode());
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
      // Try (0.,0.)
      focalMap->SetDetectorOrigin(0.0, 0.0);

    }
    else  {
      // Read boresight
      double boresightSample = inst["BoresightSample"];
      double boresightLine = inst["BoresightLine"];
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
      focalMap->SetDetectorOrigin(boresightSample, boresightLine);
    }

    // Setup detector map
    new CameraDetectorMap(this);

    // Setup distortion map
    LoMediumDistortionMap *distortionMap = new LoMediumDistortionMap(this);
    distortionMap->SetDistortion(naifIkCode());
    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    //  Determine the NAIF ID for the CK frame reference.
    if ( spacecraft.contains("3") ) {
      m_ckFrameId = -533000;
    }
    else if ( spacecraft.contains("4") ) {
      m_ckFrameId = -534000;
    }
    else if ( spacecraft.contains("5") ) {
      m_ckFrameId = -535000;
    }
    else {
      QString msg = "File does not appear to be an LunarOrbiter 3,4,5 image";
      throw IException(IException::User, msg, _FILEINFO_);
    }


    setTime(time);
    LoadCache();
    NaifStatus::CheckErrors();
  }

  
  /**
   * Returns the shutter open and close times. The user should pass in the
   * exposure duration in seconds and the StartTime keyword value, converted to
   * ephemeris time. The StartTime keyword value from the labels represents the
   * shutter center time of the observation. To find the shutter open and close
   * times, half of the exposure duration is subtracted from and added to the
   * input time parameter, respectively.  This method overrides the
   * FramingCamera class method.
   * @b Note: Lunar Orbitar did not provide exposure duration in the support
   * data.
   *
   * @param exposureDuration Exposure duration, in seconds
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @author 2011-05-03 Jeannie Walldren
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Original version.
   */
  pair<iTime, iTime> LoMediumCamera::ShutterOpenCloseTimes(double time,
                                                           double exposureDuration) {
    pair<iTime, iTime> shuttertimes;
    // To get shutter start (open) time, subtract half the exposure duration from the center time
    shuttertimes.first = time - (exposureDuration / 2);
    // To get shutter end (close) time, add half the exposure duration to the center time
    shuttertimes.second = time + (exposureDuration / 2);
    return shuttertimes;
  }
}


/**
 * This is the function that is called in order to instantiate a LoMediumCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* LoMediumCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed Lo
 *            namespace.
 */
extern "C" Isis::Camera *LoMediumCameraPlugin(Isis::Cube &cube) {
  return new Isis::LoMediumCamera(cube);
}
