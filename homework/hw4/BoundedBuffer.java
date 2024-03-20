package hw4;

public class BoundedBuffer {

  private static final int MAX_SIZE = 1000;
  private int[] elements;

  public BoundedBuffer() {
    elements = new int[MAX_SIZE];
  }
}
