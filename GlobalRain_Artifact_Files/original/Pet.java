/**
 * Pet.java
 * Original artifact from IT 145 (Global Rain).
 *
 * Base class for all pets in the Global Rain boarding system. Provides
 * default fee calculations (used as-is for cats) that the Dog subclass
 * overrides with weight-based pricing.
 */
public class Pet {
    private String name;
    private double weight;
    private int stayDays;

    public Pet(String name, double weight, int stayDays) {
        this.name = name;
        this.weight = weight;
        this.stayDays = stayDays;
    }

    public String getName() {
        return name;
    }

    public double getWeight() {
        return weight;
    }

    public int getStayDays() {
        return stayDays;
    }

    // Default grooming fee. Cats are not groomed, so this is 0.
    public double calculateGroomingFee() {
        return 0.0;
    }

    // Default stay fee. Used by cats.
    public double calculateStayFee() {
        return 18.00;
    }

    public double getTotalFee() {
        return calculateGroomingFee() + calculateStayFee();
    }
}
