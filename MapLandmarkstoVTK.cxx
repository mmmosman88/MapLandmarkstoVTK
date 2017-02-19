#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkPolyDataReader.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointLocator.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm> 


int main ( int argc, char *argv[] )
{
  // Ensure a filename was specified
  if(argc < 3)
    {
    std::cerr << "Usage: " << argv[0] << " InputSurfaceFileName fcsvFileName" << endl;
    return EXIT_FAILURE;
    }

  // Get the Surface filename from the command line
  std::string inputSurfaceFilename = argv[1];

  // Get all surface data from the file
    vtkSmartPointer<vtkPolyDataReader> surfacereader =
      vtkSmartPointer<vtkPolyDataReader>::New();
    surfacereader->SetFileName(inputSurfaceFilename.c_str());
    surfacereader->Update();

  vtkPolyData* inputPolyData = surfacereader->GetOutput();
  std::cout << "Input surface has " << inputPolyData->GetNumberOfPoints() << " points." << std::endl;

  // Build a locator
  vtkPointLocator *pointLocator = vtkPointLocator::New();
  pointLocator->SetDataSet(inputPolyData);
  pointLocator->BuildLocator();

// ---------------------------------------Reading FCSV file--------------------------------------------------------------
  
  #define NB_LINES 250
  #define NB_WORDS 250

  // Get the Surface filename from the command line
  std::string landmarksFilename = argv[2];
  std::fstream fcsvfile(landmarksFilename.c_str());
  std::string line;
  std::string mot;
  std::string words[NB_LINES][NB_WORDS]; // !!!! WARNING DEFINE AND TO PROTECT IF SUPERIOR TO 20
  int i,j;
  int* landmarkPids; 
  int NbPoints;

	if(fcsvfile)
	{
		getline(fcsvfile, line);
		fcsvfile>>mot;
		while(mot=="#")
		{
			if(getline(fcsvfile, line))
				fcsvfile>>mot;
			else
				mot="#";
		}

		i=0;
		do
		{
			
			std::size_t pos_end;// = mot.find(",,");
			std::size_t pos1;
			j=0;
			do
			{
				std::size_t pos0 = 0;
				pos1 = mot.find(',');
				pos_end = mot.find(",,");
				words[i][j] = mot.substr(pos0, pos1-pos0);
				mot = mot.substr(pos1+1);
				j++;
			}
			while(pos1+1<pos_end);
			i++;
		}
		while(fcsvfile>>mot);

                landmarkPids = new int[NbPoints]; 
                NbPoints = i;

		for (int i = 0; i < NbPoints; ++i)
		{
			double x = atof(words[i][1].c_str());
			double y = atof(words[i][2].c_str());
			double z = atof(words[i][3].c_str());
                        
                        // Find closest point
                        vtkIdType ptId;
                        double p[] = {0.0, 0.0, 0.0};
                        p[0] = x; p[1] = y; p[2] = z;
                        ptId = pointLocator->FindClosestPoint(p);
                        landmarkPids[i] = ptId;

std::cout << "landmark " << i << " position " << x << "," << y << "," << z << " and the corresponding Pid is " << landmarkPids[i] << std::endl;
		}
	}
	else
	{
		std::cout<<"Error !";
	}

// ---------------------------------------Encode landmarks in  FCSV file--------------------------------------------------------------

  vtkSmartPointer<vtkDoubleArray> Array = vtkSmartPointer<vtkDoubleArray>::New();
  Array->SetNumberOfComponents(1);
  Array->SetName("landmarks");

 for(int ID = 0; ID < inputPolyData->GetNumberOfPoints(); ID++)
    {
       double exists = 0.0;
	for (int i = 0; i < NbPoints; ++i)
	{
          double diff = landmarkPids[i] - ID;
          if (diff == 0)
          {
          exists = i+1;
          std::cout << "Landmark ID " << exists << std::endl;
          break;
          } 
	}
      Array->InsertNextValue(exists);
    }

  inputPolyData->GetPointData()->AddArray(Array);
  vtkSmartPointer<vtkPolyDataWriter> polywriter = vtkSmartPointer<vtkPolyDataWriter>::New();
  polywriter->SetFileName(inputSurfaceFilename.c_str());
  polywriter->SetInputData(inputPolyData);
  polywriter->Write();

  return EXIT_SUCCESS;
}


