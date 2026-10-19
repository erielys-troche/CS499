import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

/**
 * PetTest.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * JUnit 5 tests covering the fee boundary conditions identified in the
 * Milestone One code review (20 lb / 30 lb weight cutoffs, 2-day stay
 * cutoff), plus the new input validation. Add the JUnit 5 dependency
 * (org.junit.jupiter:junit-jupiter) to run these in your IDE or with
 * Maven/Gradle.
 */
public class PetTest {

    @Test
    void smallDogGroomingFeeAtBoundary() {
        Dog dog = new Dog("Buddy", 19.99, 3);
        assertEquals(19.95, dog.calculateGroomingFee(), 0.001);
    }

    @Test
    void mediumDogGroomingFeeAtBoundary() {
        Dog dog = new Dog("Duke", 20.0, 3);
        assertEquals(24.95, dog.calculateGroomingFee(), 0.001);
    }

    @Test
    void largeDogGroomingFeeAtBoundary() {
        Dog dog = new Dog("Zeus", 30.0, 3);
        assertEquals(29.95, dog.calculateGroomingFee(), 0.001);
    }

    @Test
    void noGroomingFeeUnderTwoDayStay() {
        Dog dog = new Dog("Milo", 15, 1);
        assertEquals(0.0, dog.calculateGroomingFee(), 0.001);
    }

    @Test
    void catNeverChargedGroomingFee() {
        Cat cat = new Cat("Whiskers", 9, 5);
        assertEquals(0.0, cat.calculateGroomingFee(), 0.001);
    }

    @Test
    void catFlatStayFee() {
        Cat cat = new Cat("Whiskers", 9, 5);
        assertEquals(18.00, cat.calculateStayFee(), 0.001);
    }

    @Test
    void negativeWeightThrowsException() {
        assertThrows(IllegalArgumentException.class, () -> new Dog("Buddy", -5, 2));
    }

    @Test
    void negativeStayDaysThrowsException() {
        assertThrows(IllegalArgumentException.class, () -> new Dog("Buddy", 20, -1));
    }

    @Test
    void emptyNameThrowsException() {
        assertThrows(IllegalArgumentException.class, () -> new Dog("", 20, 2));
    }
}
