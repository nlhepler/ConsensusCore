// demo.java

import com.pacbio.analysis.ConsensusCore.*;

public class demo
{
	static
	{
		System.loadLibrary("ConsensusCore");
	}

	public static void main(String argv[]) {
		DenseMatrix dm = new DenseMatrix(10,10);
		System.out.println(dm.Get(5,5));
	}
}
