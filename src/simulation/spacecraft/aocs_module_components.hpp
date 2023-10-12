/**
 * @file aocs_module_components.hpp
 * @brief Components installed on a spacecraft
 */

#ifndef S2E_AOBC_SIMULATION_SPACECRAFT_AOCS_MODULE_COMPONENTS_HPP_
#define S2E_AOBC_SIMULATION_SPACECRAFT_AOCS_MODULE_COMPONENTS_HPP_

#include <dynamics/dynamics.hpp>
#include <library/math/vector.hpp>
#include <simulation/spacecraft/installed_components.hpp>

// CDH
#include <components/real/cdh/on_board_computer_with_c2a.hpp>
// Power
#include "../../component/power/ina260.hpp"
#include "../../component/power/power_controller.hpp"
// AOCS
#include "../../component/aocs/mpu9250_gyro_sensor.hpp"
#include "../../component/aocs/mpu9250_magnetometer.hpp"
#include "../../component/aocs/mtq_seiren.hpp"
#include "../../component/aocs/nano_ssoc_d60.hpp"
#include "../../component/aocs/oem7600.hpp"
#include "../../component/aocs/rm3100.hpp"
#include "../../component/aocs/rw0003.hpp"
#include "../../component/aocs/sagitta.hpp"
#include "../../component/aocs/stim210.hpp"
#include <components/real/aocs/mtq_magnetometer_interference.hpp>
// Propulsion
#include <components/real/propulsion/simple_thruster.hpp>
// Mission
#include <components/real/mission/telescope.hpp>
// HILS IF
#include "../../interface/hils/hils_if_driver.hpp"
// Communication
#include <components/real/communication/wings_command_sender_to_c2a.hpp>

/**
 * @class AocsModuleComponents
 * @brief Components installed on a spacecraft
 */
class AocsModuleComponents : public InstalledComponents {
 public:
  /**
   * @fn AocsModuleComponents
   * @brief Constructor
   * @param [in] dynamics: Satellite dynamics information
   * @param [in] structure: Satellite structure information
   * @param [in] local_environment: Satellite local environment information
   * @param [in] global_environment: Global environment information
   * @param [in] configuration: Simulation configuration
   * @param [in] clock_generator: Clock generator
   * @param [in] spacecraft_id: Spacecraft ID number
   */
  AocsModuleComponents(const Dynamics *dynamics, Structure *structure, const LocalEnvironment *local_environment,
                       const GlobalEnvironment *global_environment, const SimulationConfiguration *configuration, ClockGenerator *clock_generator,
                       const unsigned int spacecraft_id);
  /**
   * @fn ~AocsModuleComponents
   * @brief Destructor
   */
  ~AocsModuleComponents();

  // Override functions for InstalledComponents
  /**
   * @fn GenerateForce_b_N
   * @brief Return force generated by components in unit Newton in body fixed frame
   */
  libra::Vector<3> GenerateForce_b_N() override;
  /**
   * @fn GenerateTorque_b_Nm
   * @brief Return torque generated by components in unit Newton-meter in body fixed frame
   */
  libra::Vector<3> GenerateTorque_b_Nm() override;
  /**
   * @fn LogSetup
   * @brief Setup the logger for components
   * @param [in] logger: Logger information
   */
  void LogSetup(Logger &logger) override;

 private:
  // CDH
  ObcWithC2a *aobc_;  //!< Attitude On Board Computer
  // Power
  PowerController *power_controller_;  //!< Power switch controller
  std::vector<Ina260> ina260s_;        //!< Ina260 current sensor
  // AOCS
  Mpu9250GyroSensor *mpu9250_gyro_sensor_;     //!< MPU9250 gyro sensor
  Mpu9250Magnetometer *mpu9250_magnetometer_;  //!< MPU9250 magnetometer
  Rm3100 *rm3100_aobc_;                        //!< RM3100 magnetometer on AOBC circuit board
  Rm3100 *rm3100_external_;                    //!< External RM3100 magnetometer
  std::vector<NanoSsocD60 *> nano_ssoc_d60_;   //!< NanoSSOC-D60 sun sensor
  MtqSeiren *mtq_seiren_;                      //!< MTQ
  Oem7600 *oem7600_;                           //!< OEM7600 GNSS Receiver
  Sagitta *sagitta_;                           //!< Sagitta Star tracker
  Stim210 *stim210_;                           //!< Stim210 gyro sensor
  Rw0003 *rw0003_x_;                           //!< RW00003 Reaction wheel X axis
  Rw0003 *rw0003_y_;                           //!< RW00003 Reaction wheel Y axis
  Rw0003 *rw0003_z_;                           //!< RW00003 Reaction wheel Z axis
  // Component Interference
  MtqMagnetometerInterference* mtq_magnetometer_interference_;  //!< Additional Bias noise by MTQ-Magnetometer interference
  // Thruster
  SimpleThruster *thruster_;  //!< Thruster
  // Mission
  Telescope *telescope_;  //!< Telescope
  // HILS
  HilsPortManager *hils_port_manager_;
  HilsIfDriver *hils_if_driver_;
  // Communication
  WingsCommandSenderToC2a *wings_command_sender_to_c2a_;

  // States
  const Dynamics *dynamics_;                      //!< Dynamics information of the spacecraft
  Structure *structure_;                          //!< Structure information of the spacecraft
  const LocalEnvironment *local_environment_;     //!< Local environment information around the spacecraft
  const GlobalEnvironment *global_environment_;   //!< Global environment information
  const SimulationConfiguration *configuration_;  //!< Simulation settings
};

#endif  // S2E_AOBC_SIMULATION_SPACECRAFT_AOCS_MODULE_COMPONENTS_HPP_
