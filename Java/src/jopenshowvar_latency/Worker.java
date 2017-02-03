package jopenshowvar_latency;

public class Worker implements Runnable {
	private int t_id;
	private int t_num;
	
	public Worker(int t_id, int t_num) {
		this.t_id = t_id;
		this.t_num = t_num;
	}
	@Override
	public void run() {
		
		
		RunTest robot = new RunTest();
		robot.connect("10.0.0.1", 7000);
		
		System.out.println("thread nr: " + t_id);
		
		
		robot.testAll(2500, t_id, t_num, false);
		

		robot.disconnect();
		System.out.println("Disconnected");

	}

}
