package jopenshowvar_latency;

public class Worker implements Runnable {
	private int t_id;
	private int t_num;
	private boolean e6axis;
	private int num_tests;
	
	public Worker(int t_id, int t_num, boolean e6axis, int num_tests) {
		this.t_id = t_id;
		this.t_num = t_num;
		this.e6axis = e6axis;
		this.num_tests = num_tests;
	}
	
	@Override
	public void run() {
		RunTest robot = new RunTest();
		robot.connect("10.0.0.1", 7000);	
		robot.testAll(num_tests, t_id, t_num, e6axis);
		robot.disconnect();
	}
}
