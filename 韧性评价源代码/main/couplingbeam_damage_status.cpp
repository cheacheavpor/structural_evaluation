#include "couplingbeam_damage_status.h"


couplingbeam_damage_status::couplingbeam_damage_status(void)
{
}
double **couplingbeam_damage_status::couplingbeam_damage(double ****member_realization,double **member_vol,int number_of_floors,int number_of_realization,int number_of_couplingbeam_eachfloor,int num_couplingbeam_partition)
{
	double **floor_damage_proportion;
	int ****member_status;
	double ***member_damage_proportion; //������Ӧ6������״̬����
	double R;                                  //1-0�����
	double theta_y=0;                          //��ʼ��ÿ���ƻ�״̬����λ�úͱ�׼ֵ
	double theta_IO=0;
	double theta_p=0;
	double theta_LS=0;
	double theta_u=0;
	double std_dev_y=0;
	double std_dev_IO=0;
	double std_dev_p=0;
	double std_dev_LS=0;
	double std_dev_u=0;
	double overall_volume=0;
	string temp;                     //һ����ʱ����
	Stat S;
	ofstream output;
	int id=1;  

	floor_damage_proportion = new double *[number_of_floors] ;				//�������
	for( int i = 0 ; i < number_of_floors ; i ++ )
		 floor_damage_proportion[i] = new double [6];

	for (int flrs=0; flrs<number_of_floors; flrs++)							//��ʼ������
	{
		for (int i=0; i<6; i++)
		{
			floor_damage_proportion[flrs][i]=0;
		}
	}
	
	member_status = new int ***[number_of_floors] ;								//��������EDPs�����ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_status[i] = new int **[number_of_couplingbeam_eachfloor];
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_couplingbeam_eachfloor ; j ++ )
		member_status[i][j] = new int *[num_couplingbeam_partition] ;
	for( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_couplingbeam_eachfloor ; j ++ )
			for( int k = 0 ; k < num_couplingbeam_partition ; k ++ )
				member_status[i][j][k] = new int [number_of_realization] ;	

	member_damage_proportion = new double **[number_of_floors];										//��������ڴ�
	for( int i = 0 ; i < number_of_floors; i ++ )
		member_damage_proportion[i] = new double *[number_of_couplingbeam_eachfloor] ;
	for ( int i = 0 ; i < number_of_floors; i ++ )
		for( int j = 0 ; j < number_of_couplingbeam_eachfloor ; j ++ )
		member_damage_proportion[i][j] = new double [6] ;

	for (int flrs=0; flrs<number_of_floors;flrs++)													//��ʼ������											
	{
		for (int member=0;member<(number_of_couplingbeam_eachfloor); member++)
		{
			for (int num=0; num<6; num++)
			{
				member_damage_proportion[flrs][member][num]=0;
			}
		}
	}
	
	overall_volume=0;
	theta_y = 0.005;
	std_dev_y = 0.39;
	theta_IO = 0.009;
	std_dev_IO=0.4;
	theta_p=0.0013;
	std_dev_p=0.4;
	theta_LS=0.015;
	std_dev_LS=0.53;
	theta_u=0.025;
	std_dev_u=0.39;
	for (int num=0; num<number_of_realization; num++)
	{
		R=S.random(0,1);
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0; member<number_of_couplingbeam_eachfloor; member++)
		{
			for (int part=0; part<num_couplingbeam_partition;part++)
			{
				
					if(R<S.CDF_normal(log(member_realization[flrs][member][part][num]/theta_y)/std_dev_y,0,1))
					{											
						if(R<S.CDF_normal(log(member_realization[flrs][member][part][num]/theta_IO)/std_dev_IO,0,1))
						{
							if(R<S.CDF_normal(log(member_realization[flrs][member][part][num]/theta_p)/std_dev_p,0,1))
							{
								if(R<S.CDF_normal(log(member_realization[flrs][member][part][num]/theta_LS)/std_dev_LS,0,1))
								{
									if(R<S.CDF_normal(log(member_realization[flrs][member][part][num]/theta_u)/std_dev_u,0,1))
									{
										member_status[flrs][member][part][num] = 5;	
									}
									else
									{
										member_status[flrs][member][part][num] = 4;
									}																	
								}
								else
								{
									member_status[flrs][member][part][num] = 3;	
								}
							}
							else
							{
								member_status[flrs][member][part][num] = 2;	
							}
						}
						else
						{
							member_status[flrs][member][part][num] = 1;	
						}
					}
					else
					{
						member_status[flrs][member][part][num] = 0;	
					}
				}
			}
		}
	}
	//looking for the most severe damage
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		for (int member = 0; member<number_of_couplingbeam_eachfloor; member++)
		{
			for (int num=0; num<number_of_realization; num++)
			{
				for (int ele=0; ele<num_couplingbeam_partition-1; ele++)
				{
					if (member_status[flrs][member][0][num]<member_status[flrs][member][ele+1][num])
					{
						member_status[flrs][member][0][num]=member_status[flrs][member][ele+1][num];
					}

				}

			}

		}

	}
	//calculating the damage status of each member
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		for (int member=0;member<number_of_couplingbeam_eachfloor; member++)
		{
			for (int num=0; num<6; num++)
			{
				member_damage_proportion[flrs][member][num]=0;
			}

		}
	}
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		for (int member=0; member<number_of_couplingbeam_eachfloor; member++)
		{
			for (int num=0; num<number_of_realization; num++)
			{
				if(member_status[flrs][member][0][num]==5)
				{
					member_damage_proportion[flrs][member][5]=member_damage_proportion[flrs][member][5]+1;
				}
				if(member_status[flrs][member][0][num]==4)
				{
					member_damage_proportion[flrs][member][4]=member_damage_proportion[flrs][member][4]+1;
				}
				if(member_status[flrs][member][0][num]==3)
				{
					member_damage_proportion[flrs][member][3]=member_damage_proportion[flrs][member][3]+1;
				}
				if(member_status[flrs][member][0][num]==2)
				{
					member_damage_proportion[flrs][member][2]=member_damage_proportion[flrs][member][2]+1;
				}
				if(member_status[flrs][member][0][num]==1)
				{
					member_damage_proportion[flrs][member][1]=member_damage_proportion[flrs][member][1]+1;
				}
				if(member_status[flrs][member][0][num]==0)
				{
					member_damage_proportion[flrs][member][0]=member_damage_proportion[flrs][member][0]+1;
				}
			}
		}
	} 
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		for (int member=0; member<number_of_couplingbeam_eachfloor; member++)
		{
			for(int i=0; i<6; i++)
			{
				member_damage_proportion[flrs][member][i] = member_damage_proportion[flrs][member][i]/number_of_realization;
			}			
		}
	}

	output.open("result\\structural component damage\\couplingbeam_damage_status.txt",ios::out);
	output<<"ID"<<"	"<<"DS1"<<"	"<<"DS2"<<"	"<<"DS3"<<"	"<<"DS4"<<"	"<<"DS5"<<"	"<<"DS6"<<"\n";
	for (int flrs=0; flrs<number_of_floors;flrs++)
	{
		for (int member=0; member<(number_of_couplingbeam_eachfloor); member++)
		{output<<id<<"	";
			for(int i=0; i<6; i++)
			{
				output<<member_damage_proportion[flrs][member][i]<<"	";
			}
			output<<"\n";	
			id++;		
		}
	}
	output.close();
	//calculating the damage status of each floor
	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int i=0; i<6; i++)
		{
			floor_damage_proportion[flrs][i]=0;
		}
	}

	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int member=0;member<(number_of_couplingbeam_eachfloor); member++)
		{
			overall_volume = overall_volume+member_vol[flrs][member];
		}		
	}

	for (int flrs=0; flrs<number_of_floors; flrs++)
	{
		for (int i=0; i<6; i++)
		{
			for (int member=0; member<number_of_couplingbeam_eachfloor; member++)
			{
				floor_damage_proportion[flrs][i]=floor_damage_proportion[flrs][i]+member_damage_proportion[flrs][member][i]*member_vol[flrs][member]/overall_volume;
			}
		}
	}

	return floor_damage_proportion;
}


couplingbeam_damage_status::~couplingbeam_damage_status(void)
{
}
