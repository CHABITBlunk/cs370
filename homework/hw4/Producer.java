package hw4;

import java.util.Random;
import java.util.Thread;

public class Producer extends Thread {

  private BoundedBuffer buffer;
  private int numItemsProduced;
  private Random random;

  public Producer(BoundedBuffer b) {
    buffer = b;
    numberItemsProduced = 0;
    random = new Random();
  }

  public void run() {
    while (numItemsProduced < 1000000 && buffer.size() < buffer.MAX_SIZE) {
      buffer.add(random.nextDouble() * 100.0);
    }
  }
}
