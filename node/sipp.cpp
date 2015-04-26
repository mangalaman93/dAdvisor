app_metrics getAppMetrics(FILE** fp, FILE** fp_rtt){
        int successful_calls=0;
        char* line=NULL;
        size_t len = 0;
        ssize_t read;
        app_metrics metrics;
        if ((read = getline(&line, &len, *fp)) != -1) {
                printf("Retrieved line of length %zu :\n", read);
                printf("%s", line);
                char* tmp = strdup(line);
                const char* tok;
                int token_num=0;
                for(tok=strtok(line, ";"); tok&&*tok; tok=strtok(NULL, ";\n")){
                        token_num++;
						printf("token_num::%d\n", token_num);
                        switch(token_num)
                        {
                                case 7:   metrics.call_rate = atof(tok);break;
                                case 49:  metrics.retransmissions = atoi(tok);break;
                                case 17:  metrics.failed_calls = atoi(tok); printf("failed calls from csv::%d\n", atoi(tok));break;
                                case 15:  successful_calls = atoi(tok);break;
                                          /*int i=0;
                                          char* line_rtt=NULL;
                                          size_t len_rtt = 0;
                                          ssize_t read_rtt;
                                          metrics.max_response_time = 0.0;
                                          for(i=0; i<successful_calls; i++){
                                                 if((read_rtt=getline(&line_rtt, &len_rtt, *fp_rtt))!=-1){
                                                        printf("Retrieved line of length rtt %zu :\n", read_rtt);
                                                        printf("%s", line_rtt);
                                                        const char* tok_rtt;
                                                        int token_num_rtt=0;
                                                        for(tok_rtt=strtok(line_rtt, ";"); tok_rtt&&*tok_rtt; tok_rtt=strtok(NULL, ";\n")){
                                                                token_num_rtt++;
                                                                if(token_num_rtt == 2){
                                                                        if(metrics.max_response_time < atof(tok_rtt))
                                                                                metrics.max_response_time = atof(tok_rtt);
                                                                }
                                                        }
                                                }
                                          }
                                          break;*/
                                case 57:  metrics.watchdog_major = atoi(tok);break;
                                case 59:  metrics.watchdog_minor = atoi(tok);break;
                                default:  break;
                        }
                }
        }
        int i=0;
        char* line_rtt=NULL;
        size_t len_rtt = 0;
        ssize_t read_rtt;
        metrics.max_response_time = 0.0;
        for(i=0; i<successful_calls; i++){
        	if((read_rtt=getline(&line_rtt, &len_rtt, *fp_rtt))!=-1){
            	printf("Retrieved line of length rtt %zu :\n", read_rtt);
            	printf("%s", line_rtt);
            	const char* tok_rtt;
            	int token_num_rtt=0;
            	for(tok_rtt=strtok(line_rtt, ";"); tok_rtt&&*tok_rtt; tok_rtt=strtok(NULL, ";\n")){
            		token_num_rtt++;
                	if(token_num_rtt == 2){
                		if(metrics.max_response_time < atof(tok_rtt))
                    		metrics.max_response_time = atof(tok_rtt);
                	}
            	}
        	}
        }
        return metrics;
}