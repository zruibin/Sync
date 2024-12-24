/*
 *
 * bluetooth.cc
 *
 * Created by Ruibin.Chow on 2024/12/24.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "bluetooth.h"
#include <thread>
#include <simpleble/SimpleBLE.h>
#include "log/logging.h"

static void PrintBluetooth() {
    if (!SimpleBLE::Adapter::bluetooth_enabled()) {
        Log(DEBUG) << "Bluetooth is not enabled";
        return ;
    }
    
    // Get a list of all available adapters
    std::vector<SimpleBLE::Adapter> adapters = SimpleBLE::Adapter::get_adapters();

    // Get the first adapter
    SimpleBLE::Adapter adapter = adapters[0];
    
    adapter.set_callback_on_scan_found([](SimpleBLE::Peripheral peripheral) {
        if (peripheral.identifier().size() == 0) {
            return ;
        }
        Log(DEBUG) << "Found device: " << peripheral.identifier() << " [" << peripheral.address() << "] "
        << peripheral.rssi() << " dBm" ;
    });
    adapter.set_callback_on_scan_updated([](SimpleBLE::Peripheral peripheral) {
        if (peripheral.identifier().size() == 0) {
            return ;
        }
//        Log(DEBUG) << "Updated device: " << peripheral.identifier() << " [" << peripheral.address() << "] "
//        << peripheral.rssi() << " dBm";
    });
    adapter.set_callback_on_scan_start([]() { Log(DEBUG) << "Scan started."; });
    adapter.set_callback_on_scan_stop([]() { Log(DEBUG) << "Scan stopped."; });

    adapter.scan_for(60 * 1000);

    Log(DEBUG) << "Scan complete.";
    
    std::vector<SimpleBLE::Peripheral> peripherals = adapter.scan_get_results();
    Log(DEBUG) << "The following devices were found:";
    for (size_t i = 0; i < peripherals.size(); i++) {
        if (!peripherals[i].is_connectable() || peripherals[i].identifier().size() == 0) {
            continue;
        }

        std::string connectable_string = peripherals[i].is_connectable() ? "Connectable" : "Non-Connectable";
        std::string peripheral_string = peripherals[i].identifier() + " [" + peripherals[i].address() + "] " +
        std::to_string(peripherals[i].rssi()) + " dBm";
        
        Log(DEBUG) << "[" << i << "] " << peripheral_string << " " << connectable_string;
        
        Log(DEBUG) << "    Tx Power: " << std::dec << peripherals[i].tx_power() << " dBm";
        Log(DEBUG) << "    Address Type: " << peripherals[i].address_type();
        
        std::vector<SimpleBLE::Service> services = peripherals[i].services();
        for (auto& service : services) {
            Log(DEBUG) << "    Service UUID: " << service.uuid();
            Log(DEBUG) << "    Service data: " << service.data().toHex();
        }
        
        std::map<uint16_t, SimpleBLE::ByteArray> manufacturer_data = peripherals[i].manufacturer_data();
        for (auto& [manufacturer_id, data] : manufacturer_data) {
            Log(DEBUG) << "    Manufacturer ID: " << manufacturer_id;
            Log(DEBUG) << "    Manufacturer data: " << data.toHex();
        }
        
        Log(DEBUG) << "--------------------------------------------------------------";
    }
}

void AsyncPrintBluetooth() {
    std::thread(PrintBluetooth).detach();
}

