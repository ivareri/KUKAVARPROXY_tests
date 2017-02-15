package jopenshowvar_latency;

public class Main {	
	public static void main(String[] args) {

		// config
		int NUM_THREAD = 5;
		int num_tests = 5000;
		boolean e6axis = true;
		
		System.out.println("Thread, #Threads, Test type, E6AXIS, Test #, Time (s)");
		
		
		Thread threads[] = new Thread[NUM_THREAD];;
		
		for (int i=0; i < NUM_THREAD; i++) {
			threads[i] = new Thread(new Worker(i, NUM_THREAD, e6axis, num_tests), String.valueOf(i));
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
