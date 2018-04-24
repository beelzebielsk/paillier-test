/* Tests:
 * - Test if the chance of getting a special point really is what the
 *   user asked for. Do a few thousand trials or so, figure out the
 *   empircal chance of a special point, calculate the difference
 *   between that and the stated chance, delta.
 * - Test out encryption of some small numbers.
 * - Test out decryption of some small numbers.
 * - Test out round-trip encryption/decryption of various numbers.
 * - Test out DDLOG: see if the shares actually fulfill their roles
 *   such that their sum equals some number.
 *   - Sample a special point.
 *   - Look for the next and previous special points (next in terms of
 *     sucessive multiplications of g).
 *   - Take a point somewhere nearby one of the special points (maybe
 *     even directly on the special point). Call this the test point.
 *   - Create shares out of the exponent of this test point by
 *     creating two numbers that sum to the exponent.
 *   - If the two numbers are both before the test point then doing
 *     ddlog on both numbers should yield shares that sum up to the
 *     test point's exponent.
 *   - If the two numbers are both after the test point, then doing
 *     ddlog on both numbers should yield shares that sum up to the
 *     exponent of the special point after the test point.
 *   - If they're on opposite sides, then it shouldn't work.
 * - Test out the full multiplication procedure: multiply an encrypted
 *   number by some other number which has been split into shares,
 *   perform the multiplication, and see if the two resulting shares
 *   sum up to the multiplied number. We can do this either by
 *   - Pre-specifying the shares of the number and the input to
 *     multiply with.
 *   - Specifying the numbers to multiply, then making one number the
 *     share, the other the input, creating the shares, encrypting the
 *     input, performing the multiplication, then doing DDLOG on the
 *     shares, then summing the result of the multiplication, then
 *     decrypting it, and comparing that to the expected result.
 */
