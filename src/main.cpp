
#include <cstdio>

#include "lib/TeaCurl.h"
#include "lib/TeaPCRE.h"

int main(int argc, char const *argv[])
{
	TeaCurl tch("https://free-proxy-list.net/");
	tch.exec();
	tch.close();

	char ***result = (char***)malloc(1024 * sizeof(char***));
	unsigned int i, j, matchCount, matchCountD2;

	TeaPCRE tre;

	char *body = (char*)malloc(100000);
	memcpy(body, tch.getBody().c_str(), strlen(tch.getBody().c_str()));

	//
	// <tr><td>74.204.10.10</td><td>59535</td><td>US</td><td class='hm'>United States</td><td>elite proxy</td><td class='hm'>no</td><td class='hx'>yes</td><td class='hm'>23 seconds ago</td></tr>
	//
	
	tre.setPattern(
		"(?:<tr><td>)([\\d\\.]*)(?:<.+<td>)(\\d{2,5})(?:<.+<td class='hm'>)(.*)(?:<)", 
		PCRE_DOTALL | PCRE_CASELESS | PCRE_MULTILINE | PCRE_UNGREEDY
	);
	tre.setSubject(body);
	tre.multiFindAll(result, &matchCount, &matchCountD2, 49);

	FILE *handle = fopen("result.txt", "w");
	for (i = 0; i < matchCount; i++) {
		for (j = 1; j < matchCountD2; j++) {
			fwrite(result[i][j], strlen(result[i][j]), sizeof(char), handle);

			if (j != matchCountD2 - 1) {
				fwrite("|",1, sizeof(char), handle);
			}
		}
		fwrite("\n", 1, sizeof(char), handle);
	}
	fclose(handle);
	return 0;
}