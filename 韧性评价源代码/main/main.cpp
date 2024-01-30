#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <Windows.h>
#include "column_damage_status.h"
#include "beam_damage_status.h"
#include "couplingbeam_damage_status.h"
#include "wall_damage_status.h"
#include "nonstructure_damage_status.h"
#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include "monte_carlo.h"
using namespace Eigen; 
using namespace std;
int main()
{	
	/////////////////////////////////////////////define the variable//////////////////////////////////////////////
	ofstream output,realization_output;
	ifstream numbers_of_column, numbers_of_beam, numbers_of_wall, numbers_of_couplingbeam,numbers_of_GMs, realization_matrix, properties, edp;
	double ****member_realization;
	double **member_vol;
	double **member_height;
	double ***member_mp;
	double **column_axial;
	double **member_EI;
	double **floor_damage_proportion;
	double ***nonstructural_floor_damage_proportion;
	double ***acc_properties;
	double ***dis_properties;
	double **EDPs;
	double **Column_Edps_realization;
	double **Beam_Edps_realization;
	double **Couplingbeam_Edps_realization; 
	double **Floor_driftratio_Edps_realization;
	double **Floor_acceleration_Edps_realization;
	double **Wall_Edps_realization;
	int number_of_floors,number_of_column_ele,number_of_beam_ele,number_of_couplingbeam_ele,number_of_wall_ele;
	int number_of_column_eachfloor,number_of_beam_eachfloor,number_of_wall_eachfloor,number_of_couplingbeam_eachfloor;
	int num_beam_partition,num_column_partition,num_couplingbeam_partition;
	int number_of_realization;
	int seismic_fortification;
	int number_of_acc_type=-1;
	int number_of_dis_type=-1;
	string temp;
	column_damage_status A;
	beam_damage_status B;
	couplingbeam_damage_status C;
	wall_damage_status D;
	nonstructure_damage_status E;
	monte_carlo M;
	char c;
	double double_temp;
	int GMs;
	int structural_type;

	/////////////insert data///////////
	cout<<"�����뽨����¥����: ";
	cin>>number_of_floors;
	cout<<"�������ܿ���ȼ�: ";
	cin>>seismic_fortification;
	cout<<"���������ؿ���ģ�����: ";
	cin>>number_of_realization;
	cout<<"������ṹ���"<<"\n";
	cout<<"1Ϊ��ܽṹ��2Ϊ���-����ǽ�ṹ��";
	cin>>structural_type;
	if(structural_type==1)
	{
		cout<<"�����뵥������EDP������";
		cin>>num_column_partition;
		cout<<"�����뵥������EDP������";
		cin>>num_beam_partition;
	}
	else
	{
		cout<<"�����뵥������EDP������";
		cin>>num_column_partition;
		cout<<"�����뵥������EDP������";
		cin>>num_beam_partition;
		cout<<"�����뵥��������EDP������";
		cin>>num_couplingbeam_partition;
	}

	numbers_of_GMs.open("edps\\beam_edp.txt",ios::in);
	GMs=-1;
	if(!numbers_of_GMs)
	{
		cerr<<"cannot open the beam file"<<endl;
		return -1;
	}
	while (numbers_of_GMs.get(c))
	{
		if (c=='\n')
			GMs++;
	}
	numbers_of_GMs.close();

	numbers_of_column.open("edps\\column_edp.txt",ios::in);
	number_of_column_ele=0;
	if (!numbers_of_column)
	{
		cerr<<"cannot open the column file"<<endl;
		return -1;
	}
	getline(numbers_of_column,temp);
	while (!numbers_of_column.eof())
	{
		numbers_of_column>>double_temp;
		number_of_column_ele++;
	}
	numbers_of_column.close();

	number_of_column_ele=number_of_column_ele/GMs;
	
	number_of_column_eachfloor = number_of_column_ele/number_of_floors/num_column_partition;


	numbers_of_beam.open("edps\\beam_edp.txt",ios::in);
	number_of_beam_ele=0;
	if (!numbers_of_beam)
	{
		cerr<<"cannot open the beam file"<<endl;
		return -1;
	}
	getline(numbers_of_beam,temp);
	while (!numbers_of_beam.eof())
	{
		numbers_of_beam>>double_temp;
		number_of_beam_ele++;
	}
	numbers_of_beam.close();
	
	number_of_beam_ele=number_of_beam_ele/GMs;
	number_of_beam_eachfloor = number_of_beam_ele/number_of_floors/num_beam_partition;
	
	if(structural_type==2)
	{
	numbers_of_couplingbeam.open("edps\\couplingbeam_edp.txt",ios::in);
	number_of_couplingbeam_ele=0;
	if (!numbers_of_couplingbeam)
	{
		cerr<<"cannot open the couplingbeam file"<<endl;
		return -1;
	}
	getline(numbers_of_couplingbeam,temp);
	while (!numbers_of_couplingbeam.eof())
	{
		numbers_of_couplingbeam>>double_temp;
		number_of_couplingbeam_ele++;
	}
	numbers_of_couplingbeam.close();

	number_of_couplingbeam_ele=number_of_couplingbeam_ele/GMs;
	number_of_couplingbeam_eachfloor = number_of_couplingbeam_ele/number_of_floors/num_couplingbeam_partition;


	numbers_of_wall.open("edps\\wall_edp.txt",ios::in);
	number_of_wall_ele=0;
	if (!numbers_of_wall)
	{
		cerr<<"cannot open the wall file"<<endl;
		return -1;
	}

	getline(numbers_of_wall,temp);
	while (!numbers_of_wall.eof())
	{
		numbers_of_wall>>double_temp;
		number_of_wall_ele++;
	}
	numbers_of_wall.close();
	number_of_wall_ele=number_of_wall_ele/GMs;
	number_of_wall_eachfloor = number_of_wall_ele/number_of_floors;


	}
	
	cout<<"===================���ؿ���ģ��==================="<<"\n";
	cout<<"==================�����EDP����==================="<<"\n";
	EDPs = new double *[GMs];										//�����ڴ�
	for( int i = 0 ; i < GMs; i ++ )
		EDPs[i] = new double [(number_of_column_eachfloor*num_column_partition)*number_of_floors] ;
	edp.open("edps\\column_edp.txt",ios::in);

	getline(edp,temp);
	for (int i=0; i<GMs; i++)
	{
		for (int num=0; num<number_of_column_eachfloor*num_column_partition*number_of_floors; num++)
		{
			edp>>EDPs[i][num];
		}
	}
	edp.close();

	Column_Edps_realization = M.monte_carlo_sim(EDPs, GMs, number_of_column_ele, number_of_realization,"column");

	realization_output.open("realization_matrix\\column_realization_matrix.txt",ios::out);
	for (int i = 0; i<number_of_column_ele; i++)
	{
		for (int j=0; j<number_of_realization; j++)
		{
		realization_output<<Column_Edps_realization[i][j]<<"	";
		}
		realization_output<<"\n";
	}
	realization_output.close();

	for( int i = 0 ; i < GMs; i ++ )								//�ͷ��ڴ�
		delete[] EDPs[i];
	delete[] EDPs; 
	cout<<"==================�����EDP����==================="<<"\n";
	EDPs = new double *[GMs];										//�����ڴ�
	for( int i = 0 ; i < GMs; i ++ )
		EDPs[i] = new double [(number_of_beam_eachfloor*num_beam_partition)*number_of_floors] ;
			
	edp.open("edps\\beam_edp.txt",ios::in);
	getline(edp,temp);
	for (int i=0; i<GMs; i++)
	{
		for (int num=0; num<number_of_beam_eachfloor*num_beam_partition*number_of_floors; num++)
		{
			edp>>EDPs[i][num];
		}
	}
	edp.close();




	Beam_Edps_realization = M.monte_carlo_sim(EDPs, GMs, number_of_beam_ele, number_of_realization, "beam");

	realization_output.open("realization_matrix\\beam_realization_matrix.txt",ios::out);
	for (int i = 0; i<number_of_beam_ele; i++)
	{
		for (int j=0; j<number_of_realization; j++)
		{
			realization_output<<Beam_Edps_realization[i][j]<<"	";
		}
		realization_output<<"\n";
	}
	realization_output.close();

	for( int i = 0 ; i < GMs; i ++ )								//�ͷ��ڴ�
		delete[] EDPs[i];
	delete[] EDPs; 
	

	if (structural_type ==2)
	{
		cout<<"==================����EDP����==================="<<"\n";
		EDPs = new double *[GMs];										//�����ڴ�
		for( int i = 0 ; i < GMs; i ++ )
			EDPs[i] = new double [(number_of_couplingbeam_eachfloor*num_couplingbeam_partition)*number_of_floors] ;
			
		edp.open("edps\\couplingbeam_edp.txt",ios::in);
		getline(edp,temp);
		for (int i=0; i<GMs; i++)
		{
			for (int num=0; num<number_of_couplingbeam_eachfloor*num_couplingbeam_partition*number_of_floors; num++)
			{
				edp>>EDPs[i][num];
			}
		}
		edp.close();

		Couplingbeam_Edps_realization = M.monte_carlo_sim(EDPs, GMs, number_of_couplingbeam_ele, number_of_realization,"couplingbeam");

		realization_output.open("realization_matrix\\couplingbeam_realization_matrix.txt",ios::out);
		for (int i = 0; i<number_of_couplingbeam_ele; i++)
		{
			for (int j=0; j<number_of_realization; j++)
			{
				realization_output<<Couplingbeam_Edps_realization[i][j]<<"	";
			}
			realization_output<<"\n";
		}
		realization_output.close();

		for( int i = 0 ; i < GMs; i ++ )								//�ͷ��ڴ�
			delete[] EDPs[i];
		delete[] EDPs; 

		cout<<"==================ǽ��EDP����==================="<<"\n";
		EDPs = new double *[GMs];										//�����ڴ�
		for( int i = 0 ; i < GMs; i ++ )
			EDPs[i] = new double [number_of_wall_eachfloor*number_of_floors] ;
			
		edp.open("edps\\wall_edp.txt",ios::in);
		getline(edp,temp);
		for (int i=0; i<GMs; i++)
		{
			for (int num=0; num<number_of_wall_eachfloor*number_of_floors; num++)
			{
				edp>>EDPs[i][num];
			}
		}
		edp.close();


		Wall_Edps_realization = M.monte_carlo_sim(EDPs, GMs, number_of_wall_ele, number_of_realization,"wall");

		realization_output.open("realization_matrix\\wall_realization_matrix.txt",ios::out);
		for (int i = 0; i<number_of_wall_ele; i++)
		{
			for (int j=0; j<number_of_realization; j++)
			{
				realization_output<<Wall_Edps_realization[i][j]<<"	";
			}
			realization_output<<"\n";
		}
		realization_output.close();

		for( int i = 0 ; i < GMs; i ++ )								//�ͷ��ڴ�
			delete[] EDPs[i];
		delete[] EDPs; 

	}
	
	cout<<"==================¥��λ�ƽ�EDP����==================="<<"\n";
	EDPs = new double *[GMs];										//�����ڴ�
	for( int i = 0 ; i < GMs; i ++ )
		EDPs[i] = new double [number_of_floors] ;
			
	edp.open("edps\\floor_driftratio_edp.txt",ios::in);
	getline(edp,temp);
	for (int i=0; i<GMs; i++)
	{
		for (int num=0; num<number_of_floors; num++)
		{
			edp>>EDPs[i][num];
		}
	}
	edp.close();

	Floor_driftratio_Edps_realization = M.monte_carlo_sim(EDPs, GMs, number_of_floors, number_of_realization,"driftratio");

	realization_output.open("realization_matrix\\floor_driftratio_realization_matrix.txt",ios::out);
	for (int i = 0; i<number_of_floors; i++)
	{
		for (int j=0; j<number_of_realization; j++)
		{
			realization_output<<Floor_driftratio_Edps_realization[i][j]<<"	";
		}
		realization_output<<"\n";
	}
	realization_output.close();

	for( int i = 0 ; i < GMs; i ++ )								//�ͷ��ڴ�
		delete[] EDPs[i];
	delete[] EDPs; 
	cout<<"==================¥����ٶ�EDP����==================="<<"\n";

	EDPs = new double *[GMs];										//�����ڴ�
	for( int i = 0 ; i < GMs; i ++ )
		EDPs[i] = new double [number_of_floors] ;

	edp.open("edps\\floor_acceleration_edp.txt",ios::in);			
	getline(edp,temp);
	for (int i=0; i<GMs; i++)
	{
		for (int num=0; num<number_of_floors; num++)
		{
			edp>>EDPs[i][num];
		}
	}
	edp.close();

	Floor_acceleration_Edps_realization = M.monte_carlo_sim(EDPs, GMs, number_of_floors, number_of_realization,"flooracceleration");

	realization_output.open("realization_matrix\\floor_acceleration_realization_matrix.txt",ios::out);
	for (int i = 0; i<number_of_floors; i++)
	{
		for (int j=0; j<number_of_realization; j++)
		{
			realization_output<<Floor_acceleration_Edps_realization[i][j]<<"	";
		}
		realization_output<<"\n";
	}
	realization_output.close();

	for( int i = 0 ; i < GMs; i ++ )								//�ͷ��ڴ�
		delete[] EDPs[i];
	delete[] EDPs; 

	cout<<"===============���������ƻ�ȷ��==============="<<"\n";

	//////////////////////�����ڴ�////////////////////////////////////////
	
	member_realization = new double ***[number_of_floors] ;								//��������EDPs�����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_realization[i] = new double **[number_of_column_eachfloor];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_column_eachfloor ; j ++ )
		 member_realization[i][j] = new double *[1] ;
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_column_eachfloor ; j ++ )
			for( int k = 0 ; k < num_column_partition ; k ++ )
				member_realization[i][j][k] = new double [number_of_realization] ;

	member_vol = new double *[number_of_floors];										//���빹����������ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_vol[i] = new double [number_of_column_eachfloor] ;

	member_height = new double *[number_of_floors];										//���빹������߶Ⱦ����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_height[i] = new double [number_of_column_eachfloor] ;

	member_mp = new double **[number_of_floors];										//���빹����ؾ����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_mp[i] = new double *[number_of_column_eachfloor] ;
	for ( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_column_eachfloor ; j ++ )
		member_mp[i][j] = new double [num_column_partition] ;

	column_axial = new double *[number_of_floors];										//��������ѹ�Ⱦ����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		column_axial[i] = new double [number_of_column_eachfloor] ;

	member_EI = new double *[number_of_floors];											//��������������նȾ����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_EI[i] = new double [number_of_column_eachfloor] ;

	floor_damage_proportion = new double *[number_of_floors];							//����¥���ƻ�״̬����
	for( int i = 0 ; i < number_of_floors; i ++ )
		floor_damage_proportion[i] = new double [6] ;

	realization_matrix.open("realization_matrix\\column_realization_matrix.txt",ios::in);     //��ȡ���ӵ�EDPs
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0; member<number_of_column_eachfloor; member++)
		{
			for (int partition=0; partition<num_column_partition; partition++)
			{
				for (int num=0; num<number_of_realization; num++)
				{
					realization_matrix>>member_realization[flrs][member][partition][num];
				}
			}
		}
	}
	realization_matrix.close();

	properties.open("properties\\column_properties.txt",ios::in); //��ȡ��������
	getline(properties,temp);
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member_type=0; member_type<number_of_column_eachfloor; member_type++)
		{
			properties>>temp;
			properties>>member_vol[flrs][member_type];
			properties>>member_height[flrs][member_type];
			for (int num=0; num<num_column_partition; num++)
			{
			properties>>member_mp[flrs][member_type][num];
			}
			properties>>column_axial[flrs][member_type];
			properties>>member_EI[flrs][member_type];	
		}
	}
	properties.close();
		
	floor_damage_proportion = A.column_damage(member_realization,seismic_fortification,member_vol,member_mp,column_axial,member_height,member_EI,number_of_floors,number_of_realization,number_of_column_eachfloor,num_column_partition);
		//�����������״̬
	output.open("result\\floor damage\\Storey column damage status.txt",ios::out);
	output<<"Floor"<<"	"<<"DS1"<<"	"<<"DS2"<<"	"<<"DS3"<<"	"<<"DS4"<<"	"<<"DS5"<<"	"<<"DS6"<<"\n";
	int id=1;
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		output<<id<<"	";
		for (int i=0; i<6; i++)
		{
			output<<floor_damage_proportion[flrs][i]<<"	";
		}
		output<<"\n";
		id++;
	}
	output.close();

	//////////////////////�ͷ�////////////////////////////////////////
	for( int i = 0 ; i <number_of_floors; i ++ )								//�ͷ�¥���ƻ�״̬����
		delete[] floor_damage_proportion[i];
	delete[] floor_damage_proportion; 

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�EDP����
		for( int j = 0 ; j < number_of_column_eachfloor ; j ++ )
			for( int k = 0 ; k < 1 ; k ++ )
				delete[] member_realization[i][j][k];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_column_eachfloor ; j ++ )
		 delete[] member_realization[i][j];
	for( int i = 0 ; i < number_of_floors; i ++ )
		delete[] member_realization[i];
	delete[] member_realization;

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ���������������ڴ�
		delete[] member_vol[i];
	delete[] member_vol; 

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷŹ�������߶Ⱦ����ڴ�
		delete[] member_height[i];
	delete[] member_height; 

	for ( int i = 0 ; i < number_of_floors; i ++ )								//�ͷŹ�����ؾ����ڴ�
		for( int j = 0 ; j < number_of_column_eachfloor ; j ++ )
		delete[] member_mp[i][j];	
	for( int i = 0 ; i < number_of_floors; i ++ )								
		delete[] member_mp[i];
	delete[] member_mp; 
	
	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�����ѹ�Ⱦ����ڴ�
		delete[] column_axial[i];
	delete[] column_axial; 

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�����������նȾ����ڴ�
		delete[] member_EI[i];
	delete[] member_EI; 

	cout<<"�������������״̬ȷ�����"<<"\n";
	//////////////////////�����ڴ�////////////////////////////////////////
	member_realization = new double ***[number_of_floors];								//��������EDPs�����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_realization[i] = new double **[number_of_beam_eachfloor];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_beam_eachfloor ; j ++ )
		 member_realization[i][j] = new double *[num_beam_partition] ;
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_beam_eachfloor ; j ++ )
			for( int k = 0 ; k < num_beam_partition ; k ++ )
				member_realization[i][j][k] = new double [number_of_realization] ;

	member_vol = new double *[number_of_floors];										//���빹����������ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_vol[i] = new double [number_of_beam_eachfloor] ;

	member_height = new double *[number_of_floors];										//���빹������߶Ⱦ����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_height[i] = new double [number_of_beam_eachfloor] ;

	member_mp = new double **[number_of_floors];										//���빹����ؾ����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_mp[i] = new double *[number_of_beam_eachfloor] ;
	for ( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_beam_eachfloor ; j ++ )
		member_mp[i][j] = new double [num_beam_partition] ;

	member_EI = new double *[number_of_floors];											//��������������նȾ����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_EI[i] = new double [number_of_beam_eachfloor] ;

	floor_damage_proportion = new double *[number_of_floors];							//����¥���ƻ�״̬����
	for( int i = 0 ; i < number_of_floors; i ++ )
		floor_damage_proportion[i] = new double [6] ;

	realization_matrix.open("realization_matrix\\beam_realization_matrix.txt",ios::in);//��ȡEDPs
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0; member<number_of_beam_eachfloor; member++)
		{
			for (int part=0; part<num_beam_partition; part++)
			{
				for (int num=0; num<number_of_realization; num++)
				{
					realization_matrix>>member_realization[flrs][member][part][num];
				}				
			}
		}
	}
	realization_matrix.close();
	properties.open("properties\\beam_properties.txt",ios::in); //��ȡ��������
	getline(properties,temp);
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member_type=0; member_type<number_of_beam_eachfloor; member_type++)
		{
			properties>>temp;
			properties>>member_vol[flrs][member_type];
			for (int part=0; part<num_beam_partition; part++)
			{
			properties>>member_mp[flrs][member_type][part];
			}
			properties>>member_height[flrs][member_type];
			properties>>member_EI[flrs][member_type];
		}
	}
	properties.close();

		
	floor_damage_proportion = B.beam_damage(member_realization,member_vol,member_mp,member_height,member_EI,number_of_floors, number_of_realization, number_of_beam_eachfloor, num_beam_partition, member_mp[0][0][0]);
	//�����������״̬
	output.open("result\\floor damage\\Storey beam damage status.txt",ios::out);
	output<<"Floor"<<"	"<<"DS1"<<"	"<<"DS2"<<"	"<<"DS3"<<"	"<<"DS4"<<"	"<<"DS5"<<"	"<<"DS6"<<"\n";
	id=1;
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		output<<id<<"	";
		for (int i=0; i<6; i++)
		{
			output<<floor_damage_proportion[flrs][i]<<"	";
		}
		output<<"\n";
		id++;
	}
	output.close();


	//�ͷ��ڴ�
	for( int i = 0 ; i <number_of_floors; i ++ )								//�ͷ�¥���ƻ�״̬����
		delete[] floor_damage_proportion[i];
	delete[] floor_damage_proportion; 

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�EDP����
		for( int j = 0 ; j < number_of_beam_eachfloor ; j ++ )
			for( int k = 0 ; k < num_beam_partition ; k ++ )
				delete[] member_realization[i][j][k];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_beam_eachfloor ; j ++ )
		 delete[] member_realization[i][j];
	for( int i = 0 ; i < number_of_floors; i ++ )
		delete[] member_realization[i];
	delete[] member_realization;

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ���������������ڴ�
		delete[] member_vol[i];
	delete[] member_vol; 

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷŹ�������߶Ⱦ����ڴ�
		delete[] member_height[i];
	delete[] member_height; 

	for ( int i = 0 ; i < number_of_floors; i ++ )								//�ͷŹ�����ؾ����ڴ�
		for( int j = 0 ; j < number_of_beam_eachfloor ; j ++ )
		delete[] member_mp[i][j];	
	for( int i = 0 ; i < number_of_floors; i ++ )								
		delete[] member_mp[i];
	delete[] member_mp; 
	
	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�����������նȾ����ڴ�
		delete[] member_EI[i];
	delete[] member_EI;
	cout<<"�������������״̬ȷ�����"<<"\n";
	
	if(structural_type==2)															//��ܼ���ǽ�ṹ
	{
	member_realization = new double ***[number_of_floors];								//��������EDPs�����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_realization[i] = new double **[number_of_couplingbeam_eachfloor];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_couplingbeam_eachfloor ; j ++ )
		 member_realization[i][j] = new double *[num_couplingbeam_partition] ;
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_couplingbeam_eachfloor ; j ++ )
			for( int k = 0 ; k < num_couplingbeam_partition ; k ++ )
				member_realization[i][j][k] = new double [number_of_realization] ;

	member_vol = new double *[number_of_floors];										//���빹����������ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_vol[i] = new double [number_of_couplingbeam_eachfloor] ;

	floor_damage_proportion = new double *[number_of_floors];							//����¥���ƻ�״̬����
	for( int i = 0 ; i < number_of_floors; i ++ )
		floor_damage_proportion[i] = new double [6] ;

	realization_matrix.open("realization_matrix\\couplingbeam_realization_matrix.txt",ios::in);//read the realization matrix 
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0; member<number_of_couplingbeam_eachfloor; member++)
		{
			for (int ele=0; ele<num_couplingbeam_partition; ele++)
			{
				for (int num=0; num<number_of_realization; num++)
				{
					realization_matrix>>member_realization[flrs][member][ele][num];
				}				
			}
		}
	}
	realization_matrix.close();

	properties.open("properties\\couplingbeam_properties.txt",ios::in); //��ȡ��������
	getline(properties,temp);
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member_type=0; member_type<number_of_couplingbeam_eachfloor; member_type++)
		{	properties>>temp;
			properties>>member_vol[flrs][member_type];
		}
	}
	properties.close();

	floor_damage_proportion = C.couplingbeam_damage(member_realization,member_vol,number_of_floors,number_of_realization,number_of_couplingbeam_eachfloor,num_couplingbeam_partition);


	output.open("result\\floor damage\\Storey couplingbeam damage status.txt",ios::out);
	output<<"Floor"<<"	"<<"DS1"<<"	"<<"DS2"<<"	"<<"DS3"<<"	"<<"DS4"<<"	"<<"DS5"<<"	"<<"DS6"<<"\n";
	int id=1;
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		output<<id<<"	";
		for (int i=0; i<6; i++)
		{
			output<<floor_damage_proportion[flrs][i]<<"	";
		}
		output<<"\n";
		id++;
	}
	output.close();
	//�ͷ��ڴ�
	for( int i = 0 ; i <number_of_floors; i ++ )								//�ͷ�¥���ƻ�״̬����
		delete[] floor_damage_proportion[i];
	delete[] floor_damage_proportion; 

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�EDP����
		for( int j = 0 ; j < number_of_couplingbeam_eachfloor ; j ++ )
			for( int k = 0 ; k < num_couplingbeam_partition ; k ++ )
				delete[] member_realization[i][j][k];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_couplingbeam_eachfloor ; j ++ )
		 delete[] member_realization[i][j];
	for( int i = 0 ; i < number_of_floors; i ++ )
		delete[] member_realization[i];
	delete[] member_realization;

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ���������������ڴ�
		delete[] member_vol[i];
	delete[] member_vol; 

	cout<<"������������״̬ȷ�����"<<"\n";

	member_realization = new double ***[number_of_floors];								//��������EDPs�����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_realization[i] = new double **[number_of_wall_eachfloor];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_wall_eachfloor ; j ++ )
		 member_realization[i][j] = new double *[1] ;
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_wall_eachfloor ; j ++ )
			for( int k = 0 ; k < 1 ; k ++ )
				member_realization[i][j][k] = new double [number_of_realization] ;

	floor_damage_proportion = new double *[number_of_floors];							//����¥���ƻ�״̬����
	for( int i = 0 ; i < number_of_floors; i ++ )
		floor_damage_proportion[i] = new double [6] ;


	member_vol = new double *[number_of_floors];										//���빹����������ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_vol[i] = new double [number_of_wall_eachfloor] ;

	realization_matrix.open("realization_matrix\\wall_realization_matrix.txt",ios::in);//read the realization matrix 
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0; member<number_of_wall_eachfloor; member++)
		{
			for (int ele=0; ele<1; ele++)
			{
				for (int num=0; num<number_of_realization; num++)
				{
					realization_matrix>>member_realization[flrs][member][ele][num];
				}				
			}
		}
	}
	realization_matrix.close();

	properties.open("properties\\wall_properties.txt",ios::in); //��ȡ��������
	getline(properties,temp);
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member_type=0; member_type<number_of_wall_eachfloor; member_type++)
		{
			properties>>temp;
			properties>>member_vol[flrs][member_type];
		}
	}
	properties.close();
	
	floor_damage_proportion = D.wall_damage(member_realization,member_vol,number_of_floors,number_of_realization,number_of_wall_eachfloor);

	//���ǽ������״̬
	output.open("result\\floor damage\\Storey wall damage status.txt",ios::out);
	output<<"Floor"<<"	"<<"DS1"<<"	"<<"DS2"<<"	"<<"DS3"<<"	"<<"DS4"<<"	"<<"DS5"<<"	"<<"DS6"<<"\n";
	id=1;
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		output<<id<<"	";
		for (int i=0; i<6; i++)
		{
			output<<floor_damage_proportion[flrs][i]<<"	";
		}
		output<<"\n";
		id++;
	}
	output.close();
	//�ͷ��ڴ�
	for( int i = 0 ; i <number_of_floors; i ++ )								//�ͷ�¥���ƻ�״̬����
		delete[] floor_damage_proportion[i];
	delete[] floor_damage_proportion; 

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�EDP����
		for( int j = 0 ; j < number_of_wall_eachfloor ; j ++ )
			for( int k = 0 ; k < 1 ; k ++ )
				delete[] member_realization[i][j][k];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_wall_eachfloor ; j ++ )
		 delete[] member_realization[i][j];
	for( int i = 0 ; i < number_of_floors; i ++ )
		delete[] member_realization[i];
	delete[] member_realization;

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ���������������ڴ�
		delete[] member_vol[i];
	delete[] member_vol; 
	cout<<"ǽ�幹������״̬ȷ�����"<<"\n";
	}

	
	//////////////////////�����ڴ�////////////////////////////////////////

	properties.open("properties\\acc_nonstructure.txt",ios::in);

	if (!properties)
	{
		cerr<<"cannot open the parawall_volume file"<<endl;
		return -1;
	}
	while (properties.get(c))
	{
		if (c=='\n')
			number_of_acc_type++;
	}
	properties.close();

	acc_properties = new double **[number_of_acc_type];
	for (int i=0; i< number_of_acc_type; i++)
		acc_properties[i]=new double *[2];
	for (int i=0 ; i<number_of_acc_type;i++)
		for (int j=0; j<2; j++)
			acc_properties[i][j]= new double [3];

	properties.open("properties\\acc_nonstructure.txt",ios::in);
	getline(properties,temp);
	for (int i=0; i<number_of_acc_type;i++)
	{properties>>temp;
		for (int j=0; j<2;j++)
		{
			for (int k=0; k<3; k++)
			{
				properties>>acc_properties[i][j][k];
			}
		}		
	}
	properties.close();


	member_realization = new double ***[number_of_floors];								//��������EDPs�����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_realization[i] = new double **[1];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < 1 ; j ++ )
		 member_realization[i][j] = new double *[1] ;
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < 1 ; j ++ )
			for( int k = 0 ; k < 1 ; k ++ )
				member_realization[i][j][k] = new double [number_of_realization] ;




	nonstructural_floor_damage_proportion = new double **[number_of_acc_type];							//����¥���ƻ�״̬����
	for( int i = 0 ; i < number_of_acc_type; i ++ )
		nonstructural_floor_damage_proportion[i] = new double *[number_of_floors];
	for( int i = 0 ; i < number_of_acc_type; i ++ )
		for (int j=0 ; j< number_of_floors; j++)
			nonstructural_floor_damage_proportion[i][j] = new double [4];

	realization_matrix.open("realization_matrix\\floor_acceleration_realization_matrix.txt",ios::in);//read the realization matrix 
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0; member<1; member++)
		{
			for (int ele=0; ele<1; ele++)
			{
				for (int num=0; num<number_of_realization; num++)
				{
					realization_matrix>>member_realization[flrs][member][ele][num];
				}				
			}
		}
	}
	realization_matrix.close();

	nonstructural_floor_damage_proportion = E.nonstructural_damage(member_realization,number_of_floors,number_of_realization, number_of_acc_type, acc_properties);

	output.open("result\\nonstructure component damage\\damage status of acc_nonstructure.txt",ios::out);
	for (int type=0; type<number_of_acc_type; type++)
		{output<<"�乹����ţ�"<<"	"<<type+1<<"\n";
		output<<"Floor"<<"	"<<"DS1"<<"	"<<"DS2"<<"	"<<"DS3"<<"	"<<"DS4"<<"\n";
		for (int flrs=0; flrs<number_of_floors;flrs++)
			{output<<flrs+1<<"	";
			for ( int i=0; i<4; i++)
			{
				output<<nonstructural_floor_damage_proportion[type][flrs][i]<<"	";
			}
			output<<"\n";
		}
		output<<"\n\n";
	}
	output.close();
	cout<<"���ٶ����зǽṹ��������״̬ȷ�����"<<"\n";

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�EDP����
		for( int j = 0 ; j < 1 ; j ++ )
			for( int k = 0 ; k < 1 ; k ++ )
				delete[] member_realization[i][j][k];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < 1 ; j ++ )
		 delete[] member_realization[i][j];
	for( int i = 0 ; i < number_of_floors; i ++ )
		delete[] member_realization[i];
	delete[] member_realization;

	for ( int i = 0 ; i < number_of_acc_type; i ++ )								//�ͷŹ�����ؾ����ڴ�
		for( int j = 0 ; j < number_of_floors ; j ++ )
		delete[] nonstructural_floor_damage_proportion[i][j];	
	for( int i = 0 ; i < number_of_acc_type; i ++ )								
		delete[] nonstructural_floor_damage_proportion[i];
	delete[] nonstructural_floor_damage_proportion; 

	for ( int i = 0 ; i < number_of_acc_type; i ++ )								//�ͷŹ�����ؾ����ڴ�
		for( int j = 0 ; j < 2 ; j ++ )
		delete[] acc_properties[i][j];	
	for( int i = 0 ; i < number_of_acc_type; i ++ )								
		delete[] acc_properties[i];
	delete[] acc_properties; 




	//////////////////////�����ڴ�////////////////////////////////////////

	properties.open("properties\\disp_nonstructure.txt",ios::in);

	if (!properties)
	{
		cerr<<"cannot open the parawall_volume file"<<endl;
		return -1;
	}
	while (properties.get(c))
	{
		if (c=='\n')
			number_of_dis_type++;
	}
	properties.close();

	dis_properties = new double **[number_of_dis_type];
	for (int i=0; i< number_of_dis_type; i++)
		dis_properties[i]=new double *[2];
	for (int i=0 ; i<number_of_dis_type;i++)
		for (int j=0; j<2; j++)
			dis_properties[i][j]= new double [3];

	properties.open("properties\\disp_nonstructure.txt",ios::in);
	getline(properties,temp);
	for (int i=0; i<number_of_dis_type;i++)
	{properties>>temp;
		for (int j=0; j<2;j++)
		{
			for (int k=0; k<3; k++)
			{
				properties>>dis_properties[i][j][k];
			}
		}		
	}
	properties.close();

	member_realization = new double ***[number_of_floors];								//��������EDPs�����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_realization[i] = new double **[1];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < 1 ; j ++ )
		 member_realization[i][j] = new double *[1] ;
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < 1 ; j ++ )
			for( int k = 0 ; k < 1 ; k ++ )
				member_realization[i][j][k] = new double [number_of_realization] ;

	nonstructural_floor_damage_proportion = new double **[number_of_dis_type];							//����¥���ƻ�״̬����
	for( int i = 0 ; i < number_of_dis_type; i ++ )
		nonstructural_floor_damage_proportion[i] = new double *[number_of_floors];
	for( int i = 0 ; i < number_of_dis_type; i ++ )
		for (int j=0 ; j< number_of_floors; j++)
			nonstructural_floor_damage_proportion[i][j] = new double [4];

	realization_matrix.open("realization_matrix\\floor_driftratio_realization_matrix.txt",ios::in);//read the realization matrix 
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0; member<1; member++)
		{
			for (int ele=0; ele<1; ele++)
			{
				for (int num=0; num<number_of_realization; num++)
				{
					realization_matrix>>member_realization[flrs][member][ele][num];
				}				
			}
		}
	}
	realization_matrix.close();
	
	nonstructural_floor_damage_proportion = E.nonstructural_damage(member_realization,number_of_floors,number_of_realization, number_of_dis_type, dis_properties);

	output.open("result\\nonstructure component damage\\damage status of dis_nonstructure.txt",ios::out);
	for (int type=0; type<number_of_dis_type; type++)
		{output<<"�乹����ţ�"<<"	"<<type+1<<"\n";
		output<<"Floor"<<"	"<<"DS1"<<"	"<<"DS2"<<"	"<<"DS3"<<"	"<<"DS4"<<"\n";
		for (int flrs=0; flrs<number_of_floors;flrs++)
			{output<<flrs+1<<"	";
			for ( int i=0; i<4; i++)
			{
				output<<nonstructural_floor_damage_proportion[type][flrs][i]<<"	";
			}
			output<<"\n";
		}
		output<<"\n\n";
	}
	output.close();

	cout<<"λ�����зǽṹ��������״̬ȷ�����"<<"\n";

	for( int i = 0 ; i < number_of_floors; i ++ )								//�ͷ�EDP����
		for( int j = 0 ; j < 1 ; j ++ )
			for( int k = 0 ; k < 1 ; k ++ )
				delete[] member_realization[i][j][k];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < 1 ; j ++ )
		 delete[] member_realization[i][j];
	for( int i = 0 ; i < number_of_floors; i ++ )
		delete[] member_realization[i];
	delete[] member_realization;

	for ( int i = 0 ; i < number_of_dis_type; i ++ )								//�ͷŹ�����ؾ����ڴ�
		for( int j = 0 ; j < number_of_floors ; j ++ )
		delete[] nonstructural_floor_damage_proportion[i][j];	
	for( int i = 0 ; i < number_of_dis_type; i ++ )								
		delete[] nonstructural_floor_damage_proportion[i];
	delete[] nonstructural_floor_damage_proportion; 

	for ( int i = 0 ; i < number_of_dis_type; i ++ )								//�ͷŹ�����ؾ����ڴ�
		for( int j = 0 ; j < 2 ; j ++ )
		delete[] dis_properties[i][j];	
	for( int i = 0 ; i < number_of_dis_type; i ++ )								
		delete[] dis_properties[i];
	delete[] dis_properties; 



	cout<<"�����������!!!"<<"\n"<<"��������˳�...";

	cin>>temp;
		
}