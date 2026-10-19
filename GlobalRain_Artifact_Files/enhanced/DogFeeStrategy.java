/**
 * DogFeeStrategy.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * Fee rules for dogs, based on weight and length of stay. The magic
 * numbers from the original Dog class have been replaced with named
 * constants, so the fee schedule is self-documenting and can be updated
 * in one place instead of hunting through if-else branches.
 */
public class DogFeeStrategy implements FeeStrategy {

    private static final double SMALL_DOG_GROOMING_FEE = 19.95;
    private static final double MEDIUM_DOG_GROOMING_FEE = 24.95;
    private static final double LARGE_DOG_GROOMING_FEE = 29.95;

    private static final double SMALL_DOG_STAY_FEE = 24.00;
    private static final double MEDIUM_DOG_STAY_FEE = 29.00;
    private static final double LARGE_DOG_STAY_FEE = 34.00;

    private static final double SMALL_MEDIUM_WEIGHT_THRESHOLD = 20.0;
    private static final double MEDIUM_LARGE_WEIGHT_THRESHOLD = 30.0;
    private static final int MINIMUM_STAY_FOR_GROOMING = 2;

    @Override
    public double calculateGroomingFee(Pet pet) {
        if (pet.getStayDays() < MINIMUM_STAY_FOR_GROOMING) {
            return 0.0;
        }
        if (pet.getWeight() < SMALL_MEDIUM_WEIGHT_THRESHOLD) {
            return SMALL_DOG_GROOMING_FEE;
        } else if (pet.getWeight() < MEDIUM_LARGE_WEIGHT_THRESHOLD) {
            return MEDIUM_DOG_GROOMING_FEE;
        } else {
            return LARGE_DOG_GROOMING_FEE;
        }
    }

    @Override
    public double calculateStayFee(Pet pet) {
        if (pet.getWeight() < SMALL_MEDIUM_WEIGHT_THRESHOLD) {
            return SMALL_DOG_STAY_FEE;
        } else if (pet.getWeight() < MEDIUM_LARGE_WEIGHT_THRESHOLD) {
            return MEDIUM_DOG_STAY_FEE;
        } else {
            return LARGE_DOG_STAY_FEE;
        }
    }
}
