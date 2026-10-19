/**
 * Pet.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * Base class for all pets in the Global Rain boarding system.
 *
 * Two enhancements over the original artifact:
 *  1. The constructor now validates its inputs and throws
 *     IllegalArgumentException on bad data, instead of silently
 *     accepting a negative weight or stay length.
 *  2. Fee calculation is delegated to a FeeStrategy (Strategy design
 *     pattern) instead of being implemented directly on Pet/Dog.
 */
public class Pet {
    private final String name;
    private final double weight;
    private final int stayDays;
    private final FeeStrategy feeStrategy;

    public Pet(String name, double weight, int stayDays, FeeStrategy feeStrategy) {
        if (name == null || name.trim().isEmpty()) {
            throw new IllegalArgumentException("Pet name cannot be empty.");
        }
        if (weight <= 0) {
            throw new IllegalArgumentException("Pet weight must be greater than zero.");
        }
        if (stayDays < 0) {
            throw new IllegalArgumentException("Stay length cannot be negative.");
        }
        this.name = name;
        this.weight = weight;
        this.stayDays = stayDays;
        this.feeStrategy = feeStrategy;
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

    public double calculateGroomingFee() {
        return feeStrategy.calculateGroomingFee(this);
    }

    public double calculateStayFee() {
        return feeStrategy.calculateStayFee(this);
    }

    public double getTotalFee() {
        return calculateGroomingFee() + calculateStayFee();
    }
}
