package hw4;

import java.lang.InterruptedException;

public class BoundedBuffer {

  private int numElements;
  private Double[] elements;
  private int addIndex;
  private int removeIndex;

  public BoundedBuffer(int size) {
    numElements = 0;
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
    elements[addIndex] = d;
    addIndex = (addIndex + 1) % elements.length;
    numElements++;
    notify();
  }

  /**
   * Try to remove an element from the buffer. Wait if this cannot be
   * done.
   *
   * @return the number at elements[removeIndex]
   */
  public synchronized double remove() throws InterruptedException {
    while (isEmpty()) {
      wait();
    }
    double rtn = elements[removeIndex];
    elements[removeIndex] = null;
    removeIndex = (removeIndex + 1) % elements.length;
    numElements--;
    notify();
    return rtn;
  }

  private boolean isFull() {
    return numElements == elements.length;
  }

  private boolean isEmpty() {
    return numElements == 0;
  }
}
