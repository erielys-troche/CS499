import java.util.ArrayList;
import java.util.List;

/**
 * Main.java
 * Original artifact from IT 145 (Global Rain).
 *
 * Demonstrates the pet check-out fee calculator. No input validation
 * exists yet -- this is addressed in the Milestone Two enhancement.
 */
public class Main {
    public static void main(String[] args) {
        List<Pet> pets = new ArrayList<Pet>();
        pets.add(new Dog("Rex", 45, 3));
        pets.add(new Pet("Whiskers", 9, 4)); // cat, uses default Pet fees
        pets.add(new Dog("Milo", 15, 1));

        for (Pet pet : pets) {
            System.out.println(pet.getName() + ": $" + pet.getTotalFee());
        }
    }
}
