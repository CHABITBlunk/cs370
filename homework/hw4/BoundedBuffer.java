package hw4;

public class BoundedBuffer {

  public final int MAX_SIZE = 1000;
  private int size;
  private double[] elements;

  public BoundedBuffer() {
    super();
    size = 0;
    elements = new double[MAX_SIZE];
  }

  /**
   * Try to add an element to the buffer. Return false if this cannot be done.
   *
   * @param d double the double to add to the buffer
   * @return false if size == MAX_SIZE - 1, true otherwise
   */
  public boolean add(double d) {
    if (size == MAX_SIZE - 1)
      return false;
    elements[size] = d;
    size++;
    return true;
  }

  /**
   * Try to remove an element from the buffer. Return null if this cannot be
   * done.
   *
   * @return null if size == 0, the number at elements[size - 1] otherwise
   */
  public int remove() {
    if (size == 0)
      return null;
    int rtn = elements[size - 1];
    elements[size - 1] = null;
    size--;
    return rtn;
  }

  public int size() {
    return size;
  }
}
