/**
 * FeeStrategy.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * Strategy interface for computing a pet's grooming and stay fees. This
 * pulls fee logic out of the Pet class hierarchy so new pricing rules,
 * or an entirely new pet type, can be added by writing a new strategy
 * class instead of modifying existing ones. This directly addresses the
 * extensibility weakness flagged in the Milestone One code review.
 */
public interface FeeStrategy {
    double calculateGroomingFee(Pet pet);
    double calculateStayFee(Pet pet);
}
