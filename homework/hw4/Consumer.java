package hw4;

import java.lang.InterruptedException;
import java.util.*;

public class Consumer extends Thread {

  private BoundedBuffer buffer;
  private int numIterations;
  private double sum;

  public Consumer(BoundedBuffer b, int iterations) {
    buffer = b;
    numIterations = iterations;
    sum = 0;
  }

  public void run() {
    for (int i = 1; i <= numIterations; i++) {
      try {
        sum += buffer.remove();
        if (i % 100000 == 0) {
          System.out.printf(
              "Consumer: Consumed %,d items, Cumulative value of consumed items = %.2f\n",
              i, sum);
        }
        if (i == numIterations) {
          System.out.printf("Consumer: Finished consuming %,d items.\n", i);
        }
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
    }
  }
}
