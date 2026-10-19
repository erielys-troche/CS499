/**
 * CatFeeStrategy.java
 * ENHANCEMENT (Milestone Two — Software Design and Engineering).
 *
 * Fee rules for cats: no grooming fee, flat stay fee. Previously this
 * logic lived as "default" behavior on the Pet base class itself, which
 * mixed "what a cat is" with "how a cat is priced." Pulling it into its
 * own strategy keeps that separation clean.
 */
public class CatFeeStrategy implements FeeStrategy {

    private static final double CAT_STAY_FEE = 18.00;

    @Override
    public double calculateGroomingFee(Pet pet) {
        return 0.0;
    }

    @Override
    public double calculateStayFee(Pet pet) {
        return CAT_STAY_FEE;
    }
}
