class OpenBook {
public:
	OpenBook(OpenBook** d, int** c) {
		next = d;
		input = c;
	};
	OpenBook() {
	};
	OpenBook** next = NULL; //다음 필드
	int** input = NULL;//다음필드로 가기위한 조작
};
