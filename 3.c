/*
	第3周编程作业
*/

#include <stdio.h>

typedef struct {
    char name[20];
    int score[3];
} Student;

//    输入一个学生的数据
Student* student_input(Student *pStudent);

//    输出一个学生的数据，包括平均成绩
void student_print(const Student *pStudent);

//    计算一个学生的平均成绩
double student_average(const Student *pStudent);

//    获得学生的一个成绩
int student_get_score(const Student *pStudent, int index);

int main (int argc, const char *argv[])
{
	/*以下字符数组存储打印结果的表头*/
	char no[] = "no";
	char name[] = "name";
	char score1[] = "score1";
	char score2[] = "score2";
	char score3[] = "score3";
	char average[] = "average";
	char _min[] = "min";
	char _max[] = "max";

	int i, j;
	double sum[3] = {0};
	double avg[3];
	int min[3] = {10, 10, 10};
	int max[3] = {0, 0, 0};
	Student mydata[10];
	for (i = 0; i < 10; i++)
	{
		mydata[i] = *student_input(&mydata[i]);
		for (j = 0; j < 3; j++)
		{
			sum[j] += student_get_score(&mydata[i], j);
			avg[j] = sum[j]/(i + 1);
			if (student_get_score(&mydata[i], j) > max[j])
			{
				max[j] = student_get_score(&mydata[i], j);
			}
			if (student_get_score(&mydata[i], j) < min[j])
			{
				min[j] = student_get_score(&mydata[i], j);
			}
		}
	}

	printf("---|--------------------|----------|----------|----------|----------|\n");
	printf("%2s |%19s |%9s |%9s |%9s |%9s |\n", no, name, score1, score2, score3, average);

	for (i = 0; i < 10; i++)
	{
		printf("---|--------------------|----------|----------|----------|----------|\n");
		printf("%2d |", i+1);
		student_print(&mydata[i]);
	}
	printf("---|--------------------|----------|----------|----------|----------|\n");
	printf("   |%19s |%.7f |%.7f |%.7f |          |\n", average, avg[0], avg[1], avg[2]);
	printf("---|--------------------|----------|----------|----------|----------|\n");
	printf("   |%19s |%9d |%9d |%9d |          |\n", _min, min[0], min[1], min[2]);
	printf("---|--------------------|----------|----------|----------|----------|\n");
	printf("   |%19s |%9d |%9d |%9d |          |\n", _max, max[0], max[1], max[2]);
	printf("---|--------------------|----------|----------|----------|----------|\n");

	return 0;
}

//    输入一个学生的数据
Student* student_input(Student *pStudent)
{
	//依次输入名字、课程1、课程2、课程3
	scanf("%s %d %d %d", &pStudent->name, &pStudent->score[0], &pStudent->score[1], &pStudent->score[2]);
	return pStudent;
}

//    计算一个学生的平均成绩
double student_average(const Student *pStudent)
{
	double avg;
	avg = (double)( (pStudent->score[0]) + (pStudent->score[1]) + (pStudent->score[2]) ) / 3;
	return avg;
}

//    输出一个学生的数据，包括平均成绩
void student_print(const Student *pStudent)
{
	printf("%19s |%9d |%9d |%9d |%.7f |\n", pStudent->name, pStudent->score[0], pStudent->score[1], pStudent->score[2], student_average(pStudent));
}

//    获得学生的一个成绩
int student_get_score(const Student *pStudent, int index)
{
	return pStudent->score[index];
}