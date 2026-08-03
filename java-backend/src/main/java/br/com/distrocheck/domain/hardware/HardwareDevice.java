package br.com.distrocheck.domain.hardware;

public class HardwareDevice {
    private String pciIdentifier;
    private String vendor;
    private String model;
    private String category;

    public HardwareDevice(String pciIdentifier, String vendor, String model, String category) {
        this.pciIdentifier = pciIdentifier;
        this.vendor = vendor;
        this.model = model;
        this.category = category;
    }

    public String getPciIdentifier() {
        return pciIdentifier;
    }

    public String getVendor() {
        return vendor;
    }

    public String getModel() {
        return model;
    }

    public String getCategory() {
        return category;
    }
}