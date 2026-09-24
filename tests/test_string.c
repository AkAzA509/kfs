// #include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include "include/test_common.h"

static int g_pass = 0;
static int g_fail = 0;

static void check_results(const char *s, size_t ret, size_t expected)
{
	if (ret == expected) {
		g_pass++;
		t_print("[OK]   ");
		t_print(s);
		t_print("\n");
	} else {
		g_fail++;
		t_print("[FAIL] ");
		t_print(s);
		t_print("  expected=");
		t_print_int(expected);
		t_print(" res=");
		t_print_int(ret);
		t_print("\n");
	}
}

static void check_condition(const char *s, int condition)
{
	if (condition) {
		g_pass++;
		t_print("[OK]   ");
		t_print(s);
		t_print("\n");
	} else {
		g_fail++;
		t_print("[FAIL] ");
		t_print(s);
		t_print("\n");
	}
}

static void check_bytes(const char *s, const unsigned char *actual,
			const unsigned char *expected, size_t len)
{
	int equal = 1;

	for (size_t i = 0; i < len; ++i) {
		if (actual[i] != expected[i]) {
			equal = 0;
			break;
		}
	}

	check_condition(s, equal);
}

static void run_memset()
{
	print_func_name("memset");
	unsigned char buffer[8] = { 0xaa, 0xaa, 0xaa, 0xaa,
				    0xaa, 0xaa, 0xaa, 0xaa };
	unsigned char large_buffer[256];
	unsigned char large_expected[256];
	const unsigned char expected[8] = { 0x23, 0x23, 0x23, 0x23,
					    0xaa, 0xaa, 0xaa, 0xaa };

	check_condition("memset returns destination",
			memset(buffer, 0x123, 4) == buffer);
	check_bytes("memset fills and preserves the remaining bytes", buffer,
		    expected, sizeof(buffer));
	check_condition("memset with zero length returns destination",
			memset(buffer, 0, 0) == buffer);

	for (size_t i = 0; i < sizeof(large_buffer); ++i)
		large_expected[i] = 0xff;
	check_condition("memset fills a large buffer",
			memset(large_buffer, -1, sizeof(large_buffer)) ==
				large_buffer);
	check_bytes("memset handles 0xff at every position", large_buffer,
		    large_expected, sizeof(large_buffer));
}

static void run_memcpy()
{
	print_func_name("memcpy");
	const unsigned char source[5] = { 0x00, 0x7f, 0x80, 0xff, 0x42 };
	unsigned char destination[8] = { 0xaa, 0xaa, 0xaa, 0xaa,
					 0xaa, 0xaa, 0xaa, 0xaa };
	unsigned char large_source[256];
	unsigned char large_destination[257];
	const unsigned char expected[8] = { 0x00, 0x7f, 0x80, 0xff,
					    0x42, 0xaa, 0xaa, 0xaa };

	check_condition("memcpy returns destination",
			memcpy(destination, source, sizeof(source)) ==
				destination);
	check_bytes("memcpy copies binary data and preserves the tail",
		    destination, expected, sizeof(destination));
	check_condition("memcpy with zero length returns destination",
			memcpy(destination, source, 0) == destination);

	for (size_t i = 0; i < sizeof(large_source); ++i)
		large_source[i] = (unsigned char)(i * 37 + 11);
	memset(large_destination, 0xaa, sizeof(large_destination));
	check_condition("memcpy copies a large buffer",
			memcpy(large_destination, large_source,
			       sizeof(large_source)) == large_destination);
	check_bytes("memcpy preserves all large-buffer bytes",
		    large_destination, large_source, sizeof(large_source));
	check_condition("memcpy does not write past the requested length",
			large_destination[sizeof(large_source)] == 0xaa);
}

static void run_memmove()
{
	print_func_name("memmove");
	unsigned char buffer[10] = "123456789";
	unsigned char large_buffer[256];
	unsigned char large_expected[256];
	const unsigned char right_expected[10] = "121234567";
	const unsigned char left_expected[10] = "345678989";

	check_condition("memmove returns destination",
			memmove(buffer + 2, buffer, 7) == buffer + 2);
	check_bytes("memmove handles overlap towards the right", buffer,
		    right_expected, sizeof(buffer));

	memcpy(buffer, "123456789", 10);
	check_condition("memmove handles zero length",
			memmove(buffer + 1, buffer, 0) == buffer + 1);
	memmove(buffer, buffer + 2, 7);
	check_bytes("memmove handles overlap towards the left", buffer,
		    left_expected, sizeof(buffer));

	for (size_t i = 0; i < sizeof(large_buffer); ++i)
		large_buffer[i] = (unsigned char)(i * 13 + 5);
	for (size_t i = 0; i < sizeof(large_expected); ++i)
		large_expected[i] = i == 0 ? large_buffer[0] :
					     large_buffer[i - 1];
	check_condition("memmove handles a one-byte right overlap",
			memmove(large_buffer + 1, large_buffer,
				sizeof(large_buffer) - 1) == large_buffer + 1);
	check_bytes("memmove preserves a large right-overlap copy",
		    large_buffer, large_expected, sizeof(large_buffer));

	for (size_t i = 0; i < sizeof(large_buffer); ++i)
		large_buffer[i] = (unsigned char)(i * 13 + 5);
	for (size_t i = 0; i < sizeof(large_expected); ++i)
		large_expected[i] = i == sizeof(large_expected) - 1 ?
					    large_buffer[i] :
					    large_buffer[i + 1];
	check_condition("memmove handles a one-byte left overlap",
			memmove(large_buffer, large_buffer + 1,
				sizeof(large_buffer) - 1) == large_buffer);
	check_bytes("memmove preserves a large left-overlap copy", large_buffer,
		    large_expected, sizeof(large_buffer));
}

static void run_memcmp()
{
	print_func_name("memcmp");
	const unsigned char first[4] = { 0x00, 0x7f, 0x80, 0xff };
	const unsigned char same[4] = { 0x00, 0x7f, 0x80, 0xff };
	const unsigned char lower[4] = { 0x00, 0x7f, 0x7f, 0xff };
	const unsigned char higher[4] = { 0x00, 0x7f, 0x81, 0xff };
	unsigned char large_first[256];
	unsigned char large_second[256];

	check_condition("memcmp reports equal buffers",
			memcmp(first, same, 4) == 0);
	check_condition("memcmp reports equality for zero length",
			memcmp(first, lower, 0) == 0);
	check_condition("memcmp compares bytes as unsigned values",
			memcmp(first, lower, 4) == 1 &&
				memcmp(first, higher, 4) == -1);
	check_condition("memcmp stops at the first difference",
			memcmp(first, lower, 2) == 0);

	for (size_t i = 0; i < sizeof(large_first); ++i) {
		large_first[i] = (unsigned char)(i * 19);
		large_second[i] = large_first[i];
	}
	check_condition("memcmp compares equal large buffers",
			memcmp(large_first, large_second,
			       sizeof(large_first)) == 0);
	large_first[0] = 0x80;
	large_second[0] = large_first[0];
	large_second[0]--;
	check_condition("memcmp detects a difference at the first byte",
			memcmp(large_first, large_second,
			       sizeof(large_first)) == 1);
	large_second[0] = large_first[0];
	large_second[sizeof(large_second) - 1]++;
	check_condition("memcmp detects a difference at the last byte",
			memcmp(large_first, large_second,
			       sizeof(large_first)) == -1);
}

// typedef void (*test_runner_t)(size_t expected, const char *s);

static void run_strlen()
{
	print_func_name("strlen");
	char *s = "Hello world!";
	check_results(s, strlen(s), 12);

	s = "\n";
	check_results(s, strlen(s), 1);

	s = "                                   ";
	check_results(s, strlen(s), 35);

	s = "0000000000000000000000000000000000000000000000000000000000000000000000000000";
	check_results(s, strlen(s), 76);

	s = "";
	check_results(s, strlen(s), 0);

	s = "\tev\34\42\23\34\5h\65\tg\edcsdf\\g\\g\\dfsdcdcsd";
	check_results(s, strlen(s), 32);

	s = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	check_results(s, strlen(s), 608);

	s = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�YP=mk��0sm��K�M�S�?+�]�3���p�`gd3�5xh�l_j~�U�[ �x��L���+:�i�Qұ��=օϸ���><-�r��iu���=�����}|�b�w�@ɳ�z�P��^4�����-f���&���:� ��7U�����boJ�[Lf�x穅�[�Ʌ��>�~�y�@���	Q�v-�͉���ܧ��뿯��]�;]Lw�]�\"2���6��:��.b\5����u�pg%h��Q�Kd@*��Jt�2�������,��Rg�Cw��x��y��jr�z�&�����9����*��v��m��ާUs0q>��&�ǅ�>�b۵=q�����C��/�\\wgl�B��s�m�Wܪ\"ĸ����H�ڮ\"/�6s�M���\\D��)�Wn�������}��/��S`D����e�������k�ѷEs1������g����Y���CH)�-^�J�S����LK��C@����ˁč����Hq";
	check_results(s, strlen(s), 1298);
}

static void run_strcmp()
{
	print_func_name("strcmp");
	char *s1 = "Hello world!";
	char *s2 = "Hello world!";
	int ret = strcmp(s1, s2);
	check_results(s1, ret, 0);

	s1 = "Hello world!";
	s2 = "Hello world";
	ret = strcmp(s1, s2);
	check_results(s1, ret, 33);

	s1 = "Hello world";
	s2 = "Hello world!";
	ret = strcmp(s1, s2);
	check_results(s1, ret, -33);

	s1 = "asdfghjkl;";
	s2 = "qwertyuiop";
	ret = strcmp(s1, s2);
	check_results(s1, ret, -16);

	s1 = "iugeofwr[ogqoefnqp;ekn;wkrnlkwnefoqih39284t03i-032-102i1[k3[ef";
	s2 = "";
	ret = strcmp(s1, s2);
	check_results(s1, ret, 105);

	s1 = "";
	s2 = "";
	ret = strcmp(s1, s2);
	check_results(s1, ret, 0);

	s1 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	s2 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	ret = strcmp(s1, s2);
	check_results(s1, ret, 0);

	s1 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	s2 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	ret = strcmp(s1, s2);
	check_results(s1, ret, 0);

	s1 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F��i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	s2 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0��P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	ret = strcmp(s1, s2);
	check_results(s1, ret, 189);
}

static void run_strncmp()
{
	print_func_name("strncmp");
	char *s1 = "Hello world!";
	char *s2 = "Hello world!";
	int ret = strncmp(s1, s2, 12);
	check_results(s1, ret, 0);

	s1 = "Hello world!";
	s2 = "Hello world!";
	ret = strncmp(s1, s2, 24);
	check_results(s1, ret, 0);

	s1 = "Hello world";
	s2 = "Hello world!";
	ret = strncmp(s1, s2, 7);
	check_results(s1, ret, 0);

	s1 = "asdfghjkl;";
	s2 = "qwertyuiop";
	ret = strncmp(s1, s2, 34);
	check_results(s1, ret, -16);

	s1 = "iugeofwr[ogqoefnqp;ekn;wkrnlkwnefoqih39284t03i-032-102i1[k3[ef";
	s2 = "";
	ret = strncmp(s1, s2, 62);
	check_results(s1, ret, 105);

	s1 = "";
	s2 = "";
	ret = strncmp(s1, s2, 1);
	check_results(s1, ret, 0);

	s1 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	s2 = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	ret = strncmp(s1, s2, 310);
	check_results(s1, ret, 0);

	s1 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	s2 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	ret = strncmp(s1, s2, 410);
	check_results(s1, ret, 0);

	s1 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F��i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	s2 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0��P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	ret = strncmp(s1, s2, 410);
	check_results(s1, ret, 189);

	s1 = "�'b�F��i�^�cZ��A|��å�_��Y�G;^�B�0���I��K�u���P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	s2 = "\[S5��%�V0�_�����Ƕ�@r�)eji�c�r�Y��'b�F2Y�����i�^�cZ��A|��å�_��Y�G;^�B�0��P�$��Ҵ��␦~��X(��␦0��ȓ���*t�m���9��]��Y��!-6)�>�,��>�65P��,h���o�Qp�D��{��E3���`��|������@7X�($�T���U���}�*�";
	ret = strncmp(s1, s2, 410);
	check_results(s1, ret, 148);

	s1 = "\tev\34\42\23\34\5h\65\tg\edcsdf\\g\\g\\dfsdcdcsd";
	s2 = "1dw32f34g54h65j678k9l0-;'[/.;o,limkunjybht";
	ret = strncmp(s1, s2, 26);
	check_results(s1, ret, -40);
}

static void check_trim(const char *orig, const char *ret, const char *expected)
{
	for (int i = 0; expected[i]; ++i) {
		if (expected[i] != ret[i]) {
			g_fail++;
			t_print("[FAIL] ");
			t_print(orig);
			t_print("  expected=");
			t_print(expected);
			t_print(" res=");
			t_print(ret);
			t_print("\n");
			return;
		}
	}

	g_pass++;
	t_print("[OK]   ");
	t_print(orig);
	t_print("\n");
}

static void run_trim()
{
	print_func_name("trim");

	char s1[] = "Hello world!";
	char *ret = trim(s1);
	check_trim(s1, ret, "Hello world!");

	char s2[] = "\n\n\nHello world!";
	ret = trim(s2);
	check_trim(s2, ret, "Hello world!");

	char s3[] = "Hello world!\n\n\n";
	ret = trim(s3);
	check_trim(s3, ret, "Hello world!");

	char s4[] = "\n\n\nHello\n\nworld!\n\n\n";
	ret = trim(s4);
	check_trim(s4, ret, "Hello\n\nworld!");

	char s5[] = "\n\t\v\f ";
	ret = trim(s5);
	check_trim(s5, ret, "");

	char s6[] = "efknwrelj\v";
	ret = trim(s6);
	check_trim(s6, ret, "efknwrelj");

	char s7[] = "\t\t\t\t\t\e ";
	ret = trim(s7);
	check_trim(s7, ret, "\e");

	char s8[] = "\t \t\f\nqwertyuiop         ";
	ret = trim(s8);
	check_trim(s8, ret, "qwertyuiop");

	char s9[] = "\t \t\f\nqwertyuiop\t\t\t\easdfghjkl;";
	ret = trim(s9);
	check_trim(s9, ret, "qwertyuiop\t\t\t\easdfghjkl;");
}

static void test_str()
{
	print_suite_name("STR FUNC");

	run_strlen();
	run_strcmp();
	run_strncmp();
	run_trim();
}

static void test_mem()
{
	print_suite_name("MEM FUNC");

	run_memset();
	run_memcpy();
	run_memmove();
	run_memcmp();
}

int main()
{
	g_pass = 0;
	g_fail = 0;

	test_str();
	test_mem();
	print_result(g_pass, g_fail);
	return g_fail != 0;
}