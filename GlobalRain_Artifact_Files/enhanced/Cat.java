/**
 * Cat.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * A cat checking into Global Rain. Previously cats had no class of
 * their own -- they were just plain Pet objects, which meant "cat
 * pricing" and "the base pet type" were the same thing. Giving cats
 * their own class makes the model clearer and matches the Dog class.
 */
public class Cat extends Pet {
    public Cat(String name, double weight, int stayDays) {
        super(name, weight, stayDays, new CatFeeStrategy());
    }
}
