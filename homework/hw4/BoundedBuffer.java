package hw4;

import java.lang.InterruptedException;

public class BoundedBuffer {

  private int length;
  private Double[] elements;

  public BoundedBuffer(int size) {
    super();
    length = 0;
    elements = new Double[size];
  }

  /**
   * Try to add an element to the buffer. Wait if this cannot be done.
   *
   * @param d double the double to add to the buffer
   */
  public synchronized void add(double d) throws InterruptedException {
    while (isFull()) {
      wait();
    }
    elements[length] = d;
    length = (length + 1) % elements.length;
    notify();
  }

  /**
   * Try to remove an element from the buffer. Return null if this cannot be
   * done.
   *
   * @return null if size == 0, the number at elements[size - 1] otherwise
   */
  public synchronized double remove() throws InterruptedException {
    while (isEmpty()) {
      wait();
    }
    double rtn = elements[length - 1];
    elements[length - 1] = null;
    length = (length - 1) % elements.length;
    notify();
    return rtn;
  }

  public boolean isFull() {
    return length == elements.length;
  }

  public boolean isEmpty() {
    return length == 0;
  }
}
