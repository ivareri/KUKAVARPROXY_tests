package jopenshowvar_latency;

import java.io.IOException;

public class Main {	
	public static void main(String[] args) {

		// config
		int NUM_THREAD = 5;
		
		// TODO Auto-generated method stub
		Thread threads[] = new Thread[NUM_THREAD];;
		
		for (int i=0; i < NUM_THREAD; i++) {
			threads[i] = new Thread(new Worker(i, NUM_THREAD), String.valueOf(i));
			threads[i].start();
		}
		
		

		
		try {
			for (int i=0; i < NUM_THREAD; i++) {
				threads[i].join();
			}
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
