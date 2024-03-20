package hw4;

import java.util.*;

public class Consumer extends Thread {

  private BoundedBuffer buffer;
  private int numItemsConsumed;
  private long sum;

  public Consumer(BoundedBuffer b) {
    buffer = b;
    numberItemsConsumed = 0;
    sum = 0;
  }

  public void run() {
    while (numItemsConsumed < 1000000 && buffer.size() > 0) {
      sum += buffer.remove();
    }
  }
}
