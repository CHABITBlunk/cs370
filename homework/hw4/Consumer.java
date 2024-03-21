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
    for (int i = 0; i < numIterations; i++) {
      try {
        double d = buffer.remove();
        sum += d;
        if (i % 100000 == 0) {
          System.out.printf(
              "Consumer: Consumed %,d items, Cumulative value of consumed items = %.2f\n",
              i, sum);
        }
        if (i == numIterations) {
          System.out.println("Consumer: finished consuming 1,000,000 items.");
        }
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
    }
  }
}
