

class SubClass{
	public int field_1 = 1337;
	public static int field_static_2 = 1337;

	SubClass(){
		System.out.println("SubClass init");
	}
	private static final SubClass instance = new SubClass();
	public int getField_1(){
		return field_1;
	}
	public static SubClass getInstance(){
		return instance;
	}
}

public class Main {
	//as u see,the value is printed
	//also work stably in higher jvm version
	private static final int field_0 = 1;
	public static void main(String[] args) throws NoSuchFieldException, InterruptedException {
		int count = 0;
		System.out.println( "[" + count+"]SubClass.getInstance().field_1 :" + SubClass.getInstance().field_1);

		System.load("E:\\Dev\\Recode-JNI\\cmake-build-debug\\Recode-JNI.dll");
		while (true){
			Thread.sleep(1000);
			count ++;
			System.out.println( "[" + count+"]SubClass.getInstance().field_1 :" + SubClass.getInstance().field_1);
			/*if (count % 5000 == 0){
				System.out.println("GC!!");
				System.gc();

			}*/
		}
	}
}
