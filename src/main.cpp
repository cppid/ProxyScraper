

#include "lib/TeaCurl.h"
#include "lib/TeaPCRE.h"

int main(int argc, char const *argv[])
{
	TeaCurl *tch = new TeaCurl("https://free-proxy-list.net/");
	tch->exec();
	tch->close();
	size_t bodyLength = strlen(tch->getBody().c_str());
	char *body = (char*)malloc(bodyLength);

	memcpy(body, tch->getBody().c_str(), bodyLength);
	delete tch;

	//
	// <tr><td>74.204.10.10</td><td>59535</td><td>US</td><td class='hm'>United States</td><td>elite proxy</td><td class='hm'>no</td><td class='hx'>yes</td><td class='hm'>23 seconds ago</td></tr>
	//

	TeaPCRE *tre = new TeaPCRE;
	tre->setPattern(
		"(?:<tr><td>)([\\d\\.]*)(?:<.+<td>)(\\d{2,5})(?:<.+<td class='hm'>)(.*)(?:<)", 
		PCRE_DOTALL | PCRE_CASELESS | PCRE_MULTILINE | PCRE_UNGREEDY
	);
	tre->setSubject(body);
	delete body;
	unsigned int i, j, matchCount, matchCountD2;
	char ***result = (char***)malloc(2000 * sizeof(char***));
	tre->multiFindAll(result, &matchCount, &matchCountD2, 49);
	delete tre;

	FILE *handle = fopen("result.txt", "w");
	for (i = 0; i < matchCount; i++) {
		for (j = 1; j < matchCountD2; j++) {
			fwrite(result[i][j], strlen(result[i][j]), sizeof(char), handle);
			printf("%s", result[i][j]);
			if (j != matchCountD2 - 1) {
				printf("|");
				fwrite("|",1, sizeof(char), handle);
			}
		}
		printf("\n");
		fwrite("\n", 1, sizeof(char), handle);
	}
	fflush(handle);
	fclose(handle);
	return 0;
}
