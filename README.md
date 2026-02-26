# LongRangeAssortativity

Under Window 11 and Visual Studio 2022, you can set up the LongRangeAssortativity project with the following steps:

1. Python: Run pip install -r requirements.txt.
2. vcpkg Integration: Ensure vcpkg is installed and run vcpkg integrate install in a Developer Command Prompt.
3. Enable Manifests:
	1. Open your project in Visual Studio 2022.
	2. Right-click the project > Properties.
	3. Go to vcpkg and set Use Vcpkg Manifest to Yes.
4. Enable OpemMP:
	1. In the same Properties window, go to C/C++ > Language.
	2. Set OpenMP Support to Yes (/openmp).
5. Enable AVX2:
	1. Still in the Properties window, go to C/C++ > Code Generation.
	2. Set Enable Enhanced Instruction Set to Advanced Vector Extensions 2 (/arch:AVX2).
	1. Set Enable Vector Length to 256 (/vlen=256)
6. Build: When you build the solution, Visual Studio will automatically detect the vcpkg.json file and install the igraph library for you.