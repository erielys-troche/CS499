import java.util.ArrayList;
import java.util.List;

/**
 * Main.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * Demonstrates the enhanced Global Rain fee calculator, including the
 * input validation added in this milestone.
 */
public class Main {
    public static void main(String[] args) {
        List<Pet> pets = new ArrayList<Pet>();
        pets.add(new Dog("Rex", 45, 3));
        pets.add(new Cat("Whiskers", 9, 4));
        pets.add(new Dog("Milo", 15, 1));

        for (Pet pet : pets) {
            System.out.printf("%s: $%.2f%n", pet.getName(), pet.getTotalFee());
        }

        // Demonstrates the new validation. In the original version, this
        // would have silently produced a nonsense fee. Now it fails fast
        // with a clear, human-readable error message instead.
        try {
            Pet invalidPet = new Dog("Buddy", -12, 2);
            System.out.println(invalidPet.getTotalFee());
        } catch (IllegalArgumentException e) {
            System.out.println("Could not check in pet: " + e.getMessage());
        }
    }
}
