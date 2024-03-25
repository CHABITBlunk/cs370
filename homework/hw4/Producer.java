package hw4;

import java.lang.InterruptedException;
import java.util.*;

public class Producer extends Thread {

  private BoundedBuffer buffer;
  private int numIterations;
  private double sum;

  public Producer(BoundedBuffer b, int iterations) {
    buffer = b;
    numIterations = iterations;
    sum = 0;
  }

  public void run() {
    Random random = new Random();
    for (int i = 1; i <= numIterations; i++) {
      double d = random.nextDouble() * 100.0;
      sum += d;
      try {
        buffer.add(d);
        if (i % 100000 == 0) {
          System.out.printf(
              "Producer: Generated %,d items, Cumulative value of generated items = %.2f\n",
              i, sum);
        }
        if (i == numIterations) {
          System.out.printf("Producer: Finished generating %,d items\n", i);
        }
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
    }
  }
}
