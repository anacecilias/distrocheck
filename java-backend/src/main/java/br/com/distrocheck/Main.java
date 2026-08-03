package br.com.distrocheck;

import br.com.distrocheck.domain.hardware.GpuDevice;
import br.com.distrocheck.domain.hardware.HardwareDevice;
import java.util.ArrayList;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        System.out.println("=========================================");
        System.out.println("   DistroCheck - Teste de Domínio (POO) ");
        System.out.println("=========================================\n");

        // 1. Instanciando objetos de hardware (Sem banco de dados, tudo em memória RAM)
        GpuDevice nvidiaGpu = new GpuDevice(
            "10de:2504", 
            "NVIDIA Corporation", 
            "GA106 [GeForce RTX 3060]", 
            true
        );

        GpuDevice intelGpu = new GpuDevice(
            "8086:9a49", 
            "Intel Corporation", 
            "Iris Xe Graphics", 
            false
        );

        // 2. Usando Polimorfismo: Guardando diferentes subclasses em uma lista do tipo Pai (HardwareDevice)
        List<HardwareDevice> listaDeHardware = new ArrayList<>();
        listaDeHardware.add(nvidiaGpu);
        listaDeHardware.add(intelGpu);

        // 3. Imprimindo as informações no terminal
        for (HardwareDevice device : listaDeHardware) {
            System.out.println("Categoria: " + device.getCategory());
            System.out.println("PCI ID: " + device.getPciIdentifier());
            System.out.println("Fabricante: " + device.getVendor());
            System.out.println("Modelo: " + device.getModel());
            
            // Verificação específica de classe usando 'instanceof' (Conceito de Java)
            if (device instanceof GpuDevice) {
                GpuDevice gpu = (GpuDevice) device;
                System.out.println("Requer Driver Proprietário? " + (gpu.isRequiresProprietaryDriver() ? "Sim" : "Não"));
            }
            
            System.out.println("-----------------------------------------");
        }
    }
}