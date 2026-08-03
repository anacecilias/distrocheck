package br.com.distrocheck.domain.hardware;

public class GpuDevice extends HardwareDevice {
    private boolean requiresProprietaryDriver;

    public GpuDevice(String pciIdentifier, String vendor, String model, boolean requiresProprietaryDriver) {
        super(pciIdentifier, vendor, model, "Placa de Vídeo (GPU)");
        this.requiresProprietaryDriver = requiresProprietaryDriver;
    }

    public boolean isRequiresProprietaryDriver() {
        return requiresProprietaryDriver;
    }
}