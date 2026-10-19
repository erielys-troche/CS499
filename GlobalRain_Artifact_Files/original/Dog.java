/**
 * Dog.java
 * Original artifact from IT 145 (Global Rain).
 *
 * Subclass of Pet with dog-specific fee calculations based on weight.
 *
 * NOTE (flagged in the Milestone One code review): fee amounts below are
 * hardcoded "magic numbers" instead of named constants, and there is no
 * validation of weight or stayDays, so invalid input (e.g. a negative
 * weight) would silently produce a fee instead of raising an error.
 */
public class Dog extends Pet {

    public Dog(String name, double weight, int stayDays) {
        super(name, weight, stayDays);
    }

    @Override
    public double calculateGroomingFee() {
        if (getStayDays() < 2) {
            return 0.0;
        }
        if (getWeight() < 20) {
            return 19.95;
        } else if (getWeight() < 30) {
            return 24.95;
        } else {
            return 29.95;
        }
    }

    @Override
    public double calculateStayFee() {
        if (getWeight() < 20) {
            return 24.00;
        } else if (getWeight() < 30) {
            return 29.00;
        } else {
            return 34.00;
        }
    }
}
