/**
 * Dog.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * A dog checking into Global Rain. All dog-specific pricing logic now
 * lives in DogFeeStrategy, so this class only wires that strategy in.
 */
public class Dog extends Pet {
    public Dog(String name, double weight, int stayDays) {
        super(name, weight, stayDays, new DogFeeStrategy());
    }
}
