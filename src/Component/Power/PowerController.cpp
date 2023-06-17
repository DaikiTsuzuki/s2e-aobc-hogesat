#include "PowerController.h"

#include <library/utilities/macros.hpp>

#include "../../Simulation/Spacecraft/aocs_module_port_config.h"

PowerController::PowerController(PowerControlUnit pcu,
                                 const std::vector<int> gpio_ports,              // GPIOのポート番号リスト
                                 const std::vector<double> output_voltage_list,  // 出力電圧リスト
                                 OnBoardComputer *obc)
    : PowerControlUnit(pcu), GpioConnectionWithObc(gpio_ports, obc), output_voltage_list_(output_voltage_list) {
  // 初期過電流閾値はテキトウに設定、最小電圧などはコンポ側で設定する
  // 5V系列
  ConnectPort((int)PowerPortIdx::INA, 1.0);
  ConnectPort((int)PowerPortIdx::MPU, 1.0);
  ConnectPort((int)PowerPortIdx::RM, 1.0);
  ConnectPort((int)PowerPortIdx::SS, 1.0);
  ConnectPort((int)PowerPortIdx::MTQ, 1.0);
  // 12V系列
  ConnectPort((int)PowerPortIdx::STIM, 1.0);
  ConnectPort((int)PowerPortIdx::STT, 1.0);
  ConnectPort((int)PowerPortIdx::OEM, 1.0);
  ConnectPort((int)PowerPortIdx::RWX, 1.0);
  ConnectPort((int)PowerPortIdx::RWY, 1.0);
  ConnectPort((int)PowerPortIdx::RWZ, 1.0);

  // All turn off
  for (int i = 0; i < (int)PowerPortIdx::MAX; i++) {
    PowerPort *power_port = GetPowerPort(i);
    power_port->SetVoltage_V(0.0);
  }

  // PICだけ特別に操作する
  ConnectPort((int)PowerPortIdx::PIC, 1.0, 3.3,
              0.43);  // TODO: 外で設定できるようにする？
  PowerPort *power_port = GetPowerPort((int)PowerPortIdx::PIC);
  power_port->SetVoltage_V(3.3);
}

void PowerController::MainRoutine(int count) {
  UNUSED(count);
  // 電源操作
#ifdef USE_HILS  // TODO GPIO操作もHILSでできるようになったら削除する
  for (int i = 0; i < (int)PowerPortIdx::MAX; i++) {
    PowerPort *power_port = GetPowerPort(i);
    power_port->SetVoltage(output_voltage_list_[i]);
  }
  return;
#else
  for (int i = 0; i < (int)PowerPortIdx::MAX; i++) {
    PowerPort *power_port = GetPowerPort(i);
    if (Read(i) == true) {
      if (i == (int)PowerPortIdx::INA) {
        power_port->SetVoltage_V(0.0);
      } else {
        power_port->SetVoltage_V(output_voltage_list_[i]);
      }
    } else {
      if (i == (int)PowerPortIdx::INA) {
        power_port->SetVoltage_V(output_voltage_list_[i]);
      } else {
        power_port->SetVoltage_V(0.0);
      }
    }
  }
  return;
#endif
}

std::string PowerController::GetLogHeader() const {
  std::string str_tmp = "";
  return str_tmp;
}
